// susfwk-core-vector.hpp
//
/*
* =====---------------- core.hpp - dynamic array ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_VECTOR_
#define _SUSFWK_CORE_VECTOR_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/allocator.hpp"
#include "susfwk/utils/container.hpp"
#include "susfwk/arch/memops.h"
#include "susfwk/arch/exit.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/core/optimization_policy.hpp"
#include "susfwk/core/system_allocator.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Dynamic continuous array of objects
	template<typename T, auto Policy = sus::optimization_policy{}, sus::allocator_base_t Alloc = sus::sysheap_allocator >
	class vector {
	public:
		using iterator = T*;
	private:
		class small_layout {
		private:
			static constexpr sus::usize small_capacity = (24 - sizeof(sus::u8)) / sizeof(T);
			union {
				struct {
					sus::usize len_;
					sus::usize cap_;
					sus::iterator_pointer_t<iterator> data_;
				} large_;
				struct {
					sus::u8 len_;
					sus::iterator_value_t<iterator> data_[small_capacity];
				} small_;
			};
		public:
			SUS_ATTRIB_PURE inline sus::usize capacity() const { return is_small() ? small_capacity : large_.cap_; }
			SUS_ATTRIB_PURE inline sus::usize size() const { return is_small() ? (sus::usize)(small_.len_) >> 1 : large_.len_ >> 1; }
			SUS_ATTRIB_PURE inline sus::iterator_pointer_t<iterator> data() const { return is_small() ? (sus::iterator_pointer_t<iterator>)(small_.data_) : large_.data_; }
			SUS_ATTRIB_PURE inline iterator begin() const { return data(); }
			SUS_ATTRIB_PURE inline iterator end() const { return data() + size(); }
			SUS_ATTRIB_PURE inline bool is_small() const { return small_.len_ & 1; }
			inline void set_size(sus::usize size) { if (is_small()) small_.len_ = ((sus::u8)size << 1) | 1; else large_.len_ = (size << 1) | 0; }
			inline void to_small() {
				assert(!is_small() && size() <= small_capacity);
				sus::u8 len = (sus::u8)(large_.len_ >> 1);
				sus::iterator_pointer_t<iterator> heap_data = large_.data_;
				sus::uninitialized_move_forward(heap_data, heap_data + len, (sus::iterator_pointer_t<iterator>)small_.data_);
				Alloc::free(heap_data);
				small_.len_ = (len << 1) | 1;
			}
			inline void to_large(sus::usize newCapacity) {
				assert(is_small() && newCapacity > small_capacity);
				sus::usize len = (sus::usize)(small_.len_ >> 1);
				sus::iterator_pointer_t<iterator> heap_data = (sus::iterator_pointer_t<iterator>)Alloc::malloc(newCapacity * sizeof(sus::iterator_value_t<iterator>));
				sus::uninitialized_move_forward((sus::iterator_pointer_t<iterator>)small_.data_, (sus::iterator_pointer_t<iterator>)(small_.data_ + len), heap_data);
				large_.data_ = heap_data;
				large_.cap_ = newCapacity;
				large_.len_ = (len << 1) | 0;
			}
			inline void set_capacity(sus::usize newCapacity) {
				assert(newCapacity != capacity() && size() <= newCapacity);
				if (is_small()) {
					if (newCapacity > small_capacity) to_large(newCapacity);
					return;
				}
				if (newCapacity > small_capacity) {
					sus::iterator_pointer_t<iterator> heap_data = (sus::iterator_pointer_t<iterator>)Alloc::malloc(newCapacity * sizeof(sus::iterator_value_t<iterator>));
					sus::uninitialized_move_forward(large_.data_, large_.data_ + size(), heap_data);
					Alloc::free(large_.data_);
					large_.data_ = heap_data;
					large_.cap_ = newCapacity;
				}
				else to_small();
			}
		public:
			inline small_layout(const small_layout&) { small_.len_ = 1; }
			inline small_layout(small_layout&& other) {
				if (other.is_small()) {
					sus::uninitialized_move_forward(other.begin(), other.end(), (iterator)small_.data_);
					small_.len_ = other.small_.len_;
					other.small_.len_ = 1;
				}
				else {
					large_ = other.large_;
					other.large_.len_ = 0;
					other.large_.cap_ = 0;
					other.large_.data_ = nullptr;
				}
			}
			inline ~small_layout() { if (!is_small()) Alloc::free(large_.data_); }
			inline small_layout() { small_.len_ = 1; }
		};
		class large_layout {
		private:
			sus::iterator_pointer_t<iterator> data_ = nullptr;
			sus::iterator_pointer_t<iterator> data_end_ = nullptr;
			sus::iterator_pointer_t<iterator> capacity_end_ = nullptr;
		public:
			SUS_ATTRIB_PURE inline sus::usize capacity() const { return capacity_end_ - data_; }
			SUS_ATTRIB_PURE inline sus::usize size() const { return data_end_ - data_; }
			SUS_ATTRIB_PURE inline sus::iterator_pointer_t<iterator> data() const { return data_; }
			SUS_ATTRIB_PURE inline iterator begin() const { return data_; }
			SUS_ATTRIB_PURE inline iterator end() const { return data_end_; }
			inline void set_size(sus::usize size) { data_end_ = data_ + size; }
			inline void set_capacity(sus::usize newCapacity) {
				assert(newCapacity != capacity() && size() <= newCapacity);
				sus::iterator_pointer_t<iterator> new_block = (sus::iterator_pointer_t<iterator>)Alloc::malloc(newCapacity * sizeof(sus::iterator_value_t<iterator>));
				if (new_block) sus::uninitialized_move_forward(begin(), end(), new_block);
				if (data_) Alloc::free(data_);
				data_end_ = new_block + size();
				data_ = new_block;
				capacity_end_ = new_block + newCapacity;
			}
		public:
			inline large_layout(const large_layout&) {}
			inline large_layout(large_layout&& other) : data_(other.data_), data_end_(other.data_end_), capacity_end_(other.capacity_end_) {
				other.data_ = nullptr;
				other.data_end_ = nullptr;
				other.capacity_end_ = nullptr;
			}
			inline ~large_layout() { if (data_) Alloc::free(data_); }
			inline large_layout() {
				if constexpr (!Policy.lazy) {
					if constexpr (Policy.data_volume == decltype(Policy)::data_volume_t::large) set_capacity(16);
					if constexpr (Policy.data_volume == decltype(Policy)::data_volume_t::medium) set_capacity(8);
					if constexpr (Policy.data_volume == decltype(Policy)::data_volume_t::tiny) set_capacity(4);
				}
			}
		};
		sus::conditional_t<Policy.data_volume != decltype(Policy)::data_volume_t::large && Policy.priority != decltype(Policy)::priority_t::speed && sizeof(sus::iterator_value_t<iterator>) <= sizeof(sus::i32), small_layout, large_layout> storage;
	private:
		SUS_ATTRIB_CONST static constexpr sus::usize grow(sus::usize size) {
			if constexpr (Policy.priority == decltype(Policy)::priority_t::speed) return size * 2;
			else if constexpr (Policy.priority == decltype(Policy)::priority_t::memory) return size * 5 / 4;
			else return size * 3 / 2;
		}
		SUS_ATTRIB_CONST static constexpr bool should_shrink(sus::usize cap, sus::usize size) {
			if constexpr (Policy.priority == decltype(Policy)::priority_t::speed) return cap > size * 8;
			else if constexpr (Policy.priority == decltype(Policy)::priority_t::memory) return cap > size * 2;
			else return cap > size * 4;
		}
	public:
		// Determine the capacity of the vector
		SUS_ATTRIB_PURE inline sus::usize capacity() const { return storage.capacity(); }
		// Get a pointer to the first element of the vector
		SUS_ATTRIB_PURE inline friend sus::iterator_pointer_t<iterator> data(const vector& vec) { return vec.storage.data(); }
		// Get the number of vector elements
		SUS_ATTRIB_PURE inline friend sus::usize size(const vector& vec) { return vec.storage.size(); }
		// Get an iterator at the beginning of the vector
		SUS_ATTRIB_PURE inline friend iterator begin(const vector& vec) { return vec.storage.begin(); }
		// Get the iterator at the end of the vector
		SUS_ATTRIB_PURE inline friend iterator end(const vector& vec) { return vec.storage.end(); }
		// Cast the vector to a pointer to the first element
		SUS_ATTRIB_PURE constexpr operator sus::iterator_pointer_t<iterator>() const { return data(*this); }
	public:
		// Reserve data size in the array
		inline void reserve(sus::usize size, iterator& i) {
			if (size > capacity()) {
				sus::usize tmp_pos = (sus::usize)(i - begin(*this));
				storage.set_capacity(grow(size));
				i = begin(*this) + tmp_pos;
			}
		}
		// Reserve data size in the array
		inline void reserve(sus::usize size) {
			if (size > capacity()) {
				storage.set_capacity(grow(size));
			}
		}
		// Compress the array - smart compess
		inline void compress() {
			if (should_shrink(capacity(), size(*this))) { storage.set_capacity(grow(size(*this))); }
		}
		// Compress the array before size
		inline void shrink_to_fit() {
			if (size(*this) != capacity()) storage.set_capacity(size(*this));
		}
	public:
		// Get a vector object by index
		SUS_ATTRIB_PURE inline sus::iterator_reference_t<iterator> operator[](sus::usize index) const { assert(index < size(*this)); return data(*this)[index]; }
		// Inserting elements in an arbitrary location
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::remove_cvref_t<T>, sus::iterator_value_t<sus::iterator_t<C>>>)
			inline iterator insert(iterator i, const C& c) {
			assert(i <= end(*this) && size(c));
			if constexpr (auto_resize) reserve(size(*this) + size(c), i);
			sus::move(i, end(*this), i + size(c));
			sus::uninitialized_copy_forward(begin(c), end(c), i);
			storage.set_size(size(*this) + size(c));
			return i;
		}
		// Inserting null in an arbitrary location
		template<bool auto_resize = true>
		inline iterator insert_uninitialized(iterator i, sus::usize n) {
			assert(i <= end(*this));
			if (!n) return i;
			if constexpr (auto_resize) reserve(size(*this) + n, i);
			sus::uninitialized_move(i, end(*this), i + n);
			storage.set_size(size(*this) + n);
			return i;
		}
		// Inserting elements in an arbitrary location
		template<bool auto_resize = true>
		inline iterator insert(iterator i, sus::add_const_t<sus::iterator_reference_t<iterator>> value, sus::usize n) {
			assert(i <= end(*this));
			if (!n) return i;
			if constexpr (auto_resize) reserve(size(*this) + n, i);
			sus::uninitialized_move(i, end(*this), i + n);
			for (auto first = i, last = i + n; first != last; first++) ::new((void*)first) sus::iterator_value_t<iterator>(value);
			storage.set_size(size(*this) + n);
			return i;
		}
		// Inserting elements in an arbitrary location
		template<bool auto_resize = true, typename U> requires (sus::is_same_v<sus::remove_cvref_t<U>, sus::iterator_value_t<iterator>>)
			inline iterator insert(iterator i, U&& value) {
			assert(i <= end(*this));
			if constexpr (auto_resize) reserve(size(*this) + 1, i);
			sus::uninitialized_move(i, end(*this), i + 1);
			::new((void*)i) sus::iterator_value_t<iterator>(sus::forward<U>(value));
			storage.set_size(size(*this) + 1);
			return i;
		}
		// Inserting an elements at the end
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::remove_cvref_t<T>, sus::iterator_value_t<sus::iterator_t<C>>>)
			inline void push(const C& c) {
			if (!size(c)) return;
			if constexpr (auto_resize) reserve(size(*this) + size(c));
			sus::uninitialized_copy_forward(begin(c), end(c), end(*this));
			storage.set_size(size(*this) + size(c));
		}
		// Inserting an elements at the end
		template<bool auto_resize = true>
		inline void push(sus::add_const_t<sus::iterator_reference_t<iterator>> value, sus::usize n) {
			if constexpr (auto_resize) reserve(size(*this) + n);
			sus::uninitialized_fill(end(*this), end(*this) + n, value);
			storage.set_size(size(*this) + n);
		}
		// Inserting an elements at the end
		template<bool auto_resize = true>
		inline void push_uninitialized(sus::usize n) {
			if constexpr (auto_resize) reserve(size(*this) + n);
			storage.set_size(size(*this) + n);
		}
		// Inserting an element at the end
		template<bool auto_resize = true, typename U> requires (sus::is_same_v<sus::remove_cvref_t<U>, sus::iterator_value_t<iterator>>)
			inline void push(U&& value) {
			if constexpr (auto_resize) reserve(size(*this) + 1);
			::new(end(*this)) sus::iterator_value_t<iterator>(sus::forward<U>(value));
			storage.set_size(size(*this) + 1);
		}
		// Insert with the constructor
		template<bool auto_resize = true, typename... Args>
		inline iterator emplace(iterator i, Args&&... args) {
			assert(i <= end(*this));
			if constexpr (auto_resize) reserve(size(*this) + 1);
			sus::uninitialized_move(i, end(*this), i + 1);
			::new((void*)i) sus::iterator_value_t<iterator>(sus::forward<Args>(args)...);
			storage.set_size(size(*this) + 1);
			return i;
		}
		// Inserting the constructor at the end
		template<bool auto_resize = true, typename... Args>
		inline void push_emplace(Args&&... args) {
			if constexpr (auto_resize) reserve(size(*this) + 1);
			::new((void*)end(*this)) sus::iterator_value_t<iterator>(sus::forward<Args>(args)...);
			storage.set_size(size(*this) + 1);
		}
		// Delete items within a range
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::remove_cvref_t<T>, sus::iterator_value_t<sus::iterator_t<C>>>)
			inline iterator erase(const C& c) {
			assert(end(c) < end(*this));
			if (!size(c)) return begin(c);
			sus::destroy(begin(c), end(c));
			sus::uninitialized_move(end(c), end(*this), begin(c));
			storage.set_size(size(*this) - size(c));
			if constexpr (auto_resize && Policy.priority == decltype(Policy)::priority_t::memory) compress();
			return begin(c);
		}
		// Delete items within a range
		template<bool auto_resize = true>
		inline iterator erase(iterator i) {
			assert(i < end(*this));
			sus::destroy(i);
			sus::uninitialized_move(i + 1, end(*this), i);
			storage.set_size(size(*this) - 1);
			if constexpr (auto_resize && Policy.priority == decltype(Policy)::priority_t::memory) compress();
			return i;
		}
		// Delete items within a range
		template<bool auto_resize = true>
		inline iterator erase(iterator i, sus::usize n) {
			assert(i + n <= end(*this));
			if (!n) return;
			sus::destroy(i, n);
			sus::uninitialized_move(i + n, end(*this), i);
			storage.set_size(size(*this) - n);
			if constexpr (auto_resize && Policy.priority == decltype(Policy)::priority_t::memory) compress();
			return i;
		}
		// Delete an element from the end
		template<bool auto_resize = true>
		inline void pop() {
			assert(size(*this));
			sus::destroy(end(*this) - 1);
			storage.set_size(size(*this) - 1);
			if constexpr (auto_resize && Policy.priority == decltype(Policy)::priority_t::memory) compress();
		}
		// Delete an element from the end
		template<bool auto_resize = true>
		inline void pop(sus::usize n) {
			assert(n <= size(*this));
			sus::destroy(end(*this) - n, n);
			storage.set_size(size(*this) - n);
			if constexpr (auto_resize && Policy.priority == decltype(Policy)::priority_t::memory) compress();
		}
		// Change the size of the array
		template<bool auto_resize = true>
		inline void resize(sus::usize newSize, sus::add_const_t<sus::iterator_reference_t<iterator>> value = sus::iterator_value_t<iterator>()) {
			if (newSize < size(*this)) erase<auto_resize>(begin(*this) + newSize, size(*this) - newSize);
			else insert<auto_resize>(end(*this), value, newSize - size(*this));
		}
		// Clean the entire array
		template<bool shrink = false>
		inline void clear() {
			sus::destroy(*this);
			storage.set_size(0);
			if constexpr (shrink) shrink_to_fit();
		}
		// Inserting copies with a zeros
		template<bool auto_resize = true>
		inline void assign(sus::add_const_t<sus::iterator_reference_t<iterator>> value, sus::usize n) { clear(); push<auto_resize>(value, n); }
		// Inserting copies with a zeros
		template<bool auto_resize = true, sus::iterator_forward_t U>
		inline void assign(U&& value) { clear(); push<auto_resize>(sus::forward<U>(value)); }
		// Inserting a range with a zero
		template<bool auto_resize = true, sus::container_t C> requires(sus::is_same_v<sus::remove_cvref_t<T>, sus::iterator_value_t<sus::iterator_t<C>>>)
		inline void assign(const C& c) { clear(); push<auto_resize>(c); }
	public:
		inline vector(const vector& other) : storage(other.storage) {
			storage.set_capacity(size(other));
			sus::uninitialized_copy_forward(begin(other), end(other), storage.data());
			storage.set_size(size(other));
		}
		inline vector& operator=(const vector& other) {
			if (this != &other) {
				clear();
				storage.set_capacity(size(other));
				sus::uninitialized_copy_forward(begin(other), end(other), storage.data());
				storage.set_size(size(other));
			}
			return *this;
		}
		inline vector(vector&& other) : storage(sus::move(other.storage)) {}
		inline vector& operator=(vector&& other) {
			if (this != &other) {
				this->~vector();
				::new(this) vector(sus::move(other));
			}
			return *this;
		}
		inline ~vector() { clear(); }
		explicit vector() : storage() {}
		inline vector(sus::usize capacity) : storage() { storage.set_capacity(capacity); }
		template<sus::container_t C> requires(sus::is_same_v<sus::remove_cvref_t<T>, sus::iterator_value_t<sus::iterator_t<C>>>)
		inline vector(const C& c) : storage() { push(c); }
	};

	template<sus::container_t C>
	vector(const C&) -> vector<sus::iterator_value_t<sus::iterator_t<C>>>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_VECTOR_ */
