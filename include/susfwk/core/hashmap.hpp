// susfwk-core-hashmap.hpp
//
/*
* =====---------------- hashmap.hpp - unordered hash map ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_HASHMAP_
#define _SUSFWK_CORE_HASHMAP_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/iterator.hpp"
#include "susfwk/utils/allocator.hpp"
#include "susfwk/core/algorithms.hpp"
#include "susfwk/arch/memops.h"
#include "susfwk/arch/exit.hpp"
#include "susfwk/arch/math.hpp"
#include "susfwk/core/optimization_policy.hpp"
#include "susfwk/core/pair.hpp"
#include "susfwk/core/hasher.hpp"
#include "susfwk/core/system_allocator.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// Key-value hash table
	template<typename K, typename V, sus::hasher_t<K> Hasher = sus::hasher<K>, sus::comparator_t<const K&> Pred = sus::equal_to<K>, auto Policy = optimization_policy{}, sus::allocator_base_t Alloc = sus::sysheap_allocator>
	class hashmap {
	public:
		// Minimum number of buckets in a hash table
		static constexpr sus::usize base_capacity = (Policy.data_volume == decltype(Policy)::data_volume_t::tiny ? 8 : (Policy.data_volume == decltype(Policy)::data_volume_t::large ? 256 : 64));
		struct bucket_entry;
		class iterator : public sus::iterator_forward_father<iterator> {
		private:
			bucket_entry* first;
			bucket_entry* last;
			constexpr void skip_empty() { while (first != last && !first->is_occupied()) first++; }
		protected:
			constexpr void increment() { ++first; skip_empty(); }
			SUS_ATTRIB_PURE constexpr sus::pair<K, V>& dereference() const { return first->value; }
		public:
			constexpr iterator(bucket_entry* f, bucket_entry* l) : first(f), last(l) { skip_empty(); }
			constexpr iterator(const hashmap& map, sus::usize i = 0) : first(map.buckets_ + i), last(map.buckets_ + map.capacity()) { skip_empty(); }
			SUS_ATTRIB_PURE constexpr bucket_entry& entry() { return *first; };
			SUS_ATTRIB_PURE constexpr bool operator==(const iterator& other) const { return first == other.first && last == other.last; }
			SUS_ATTRIB_PURE constexpr bool operator!=(const iterator& other) const { return !(*this == other); }
		};
		struct bucket_entry {
			sus::usize hash;
			enum class type_t {
				empty = 0,
				occupied,
				deleted
			} type_;
			sus::iterator_value_t<iterator> value;
			constexpr bool is_empty() { return type_ == type_t::empty; }
			constexpr bool is_occupied() { return type_ == type_t::occupied; }
			constexpr bool is_deleted() { return type_ == type_t::deleted; }
		};
	private:
		sus::usize capacity_;
		sus::usize occupied_count_ = 0;
		sus::usize deleted_count_ = 0;
		bucket_entry* buckets_;
		SUS_ATTRIB_NO_UNIQUE_ADDRESS Hasher hasher_ = Hasher();
		SUS_ATTRIB_NO_UNIQUE_ADDRESS Pred pred_ = Pred();
	private:
		SUS_ATTRIB_PURE constexpr sus::usize deleted_count() const { return deleted_count_; }
		SUS_ATTRIB_PURE constexpr sus::usize empty_count() const { return capacity_ - occupied_count_ - deleted_count_; }
		SUS_ATTRIB_CONST static constexpr sus::usize grow(sus::usize size) {
			if constexpr (Policy.priority == decltype(Policy)::priority_t::speed) return size << 3;
			else if constexpr (Policy.priority == decltype(Policy)::priority_t::memory) return size << 1;
			else return size << 2;
		}
		SUS_ATTRIB_CONST static constexpr bool should_grow(sus::usize cap, sus::usize size) {
			return size >= cap * 3 / 4;
		}
		SUS_ATTRIB_CONST static constexpr bool should_shrink(sus::usize delc, sus::usize size) {
			return delc > size;
		}
	public:
		// Get the table capacity
		SUS_ATTRIB_PURE constexpr sus::usize capacity() const { return capacity_; }
		// Get the number of items in the table
		SUS_ATTRIB_PURE constexpr friend sus::usize size(const hashmap& map) { return map.occupied_count_; }
		// Get a pointer to the beginning of the bucket array
		SUS_ATTRIB_PURE constexpr friend bucket_entry* data(const hashmap& map) { return map.buckets_; }
		// Get an iterator on the first element
		SUS_ATTRIB_PURE constexpr friend iterator begin(const hashmap& map) { return iterator(map, 0); }
		// Get the iterator for the last element
		SUS_ATTRIB_PURE constexpr friend iterator end(const hashmap& map) { return iterator(map, map.capacity()); }
		// Cast the vector to a pointer to the first element
		SUS_ATTRIB_PURE inline operator sus::iterator_pointer_t<iterator>() const { return data(*this); }
	public:
		// Recreating the entire table with a new capacity
		inline void rehash(sus::usize newCapacity) {
			assert(newCapacity >= size(*this) && sus::is_power2(newCapacity));
			hashmap tmp = hashmap(newCapacity);
			for (auto i = begin(*this); i != end(*this); ++i) {
				tmp.insert<false, false>(sus::move(i->first), sus::move(i->second));
			}
			*this = sus::move(tmp);
		}
		// Auto expansion with rehashing
		inline void reserve() {
			if (should_grow(capacity(), size(*this))) rehash(sus::max<sus::usize>((sus::usize)(capacity() << 1), (sus::usize)base_capacity));
		}
		// Reserve capacity
		inline void reserve(sus::usize newCapacity) {
			if (newCapacity <= capacity()) { compress(newCapacity); return; }
			newCapacity = newCapacity * 4 / 3;
			if (!sus::is_power2(newCapacity)) newCapacity = sus::round_to_power2(newCapacity);
			rehash(newCapacity);
		}
		// Auto compression with rehashing
		inline void compress() {
			if (should_shrink(deleted_count(), size(*this))) rehash(sus::max<sus::usize>((sus::usize)(size(*this) << 1), (sus::usize)base_capacity));
		}
		// Compress with min capacity
		inline void compress(sus::usize minCapacity) {
			if (should_shrink(deleted_count(), size(*this))) {
				if (minCapacity < base_capacity) minCapacity = base_capacity;
				else if (!sus::is_power2(minCapacity)) minCapacity = sus::round_to_power2(minCapacity);
				rehash(sus::max(size(*this) * 4 / 3, minCapacity));
			}
		}
	private:
		// The general function is common for insertion and replacement
		template<bool auto_replace = true, bool auto_rehash = true, typename KU, typename ValueCtor>
		iterator do_insert_ex(KU&& key, ValueCtor&& valueCtor) {
			if constexpr (auto_rehash) reserve();
			sus::usize hash = hasher_(key);
			sus::usize mask = capacity() - 1;
			sus::isize step = ((hash >> 16) | 1) & mask;
			if (!step) step = 1;
			for (sus::usize first = hash & mask, firstDeleted = (sus::usize)-1, i = first;;) {
				bucket_entry* entry = &buckets_[i];
				switch (entry->type_) {
				case bucket_entry::type_t::empty:
					if (firstDeleted != (sus::usize)-1) { deleted_count_--; entry = &buckets_[firstDeleted]; i = firstDeleted; }
					::new(&entry->value.first) K(sus::forward<KU>(key));
					valueCtor(entry);
					entry->hash = hash;
					entry->type_ = bucket_entry::type_t::occupied;
					occupied_count_++;
					return iterator(*this, i);
				case bucket_entry::type_t::occupied:
					if constexpr (auto_replace) {
						if (entry->hash != hash || !pred_(entry->value.first, key)) break;
						entry->value.second.~V();
						valueCtor(entry);
						return iterator(*this, i);
					}
					else return end(*this);
				case bucket_entry::type_t::deleted: if (firstDeleted == (sus::usize)-1) firstDeleted = i; break;
				}
				i = (i + step) & mask;
				if (i == first) break;
			}
			return end(*this);
		}
	public:
		//Delete by entry
		template<bool auto_rehash = true>
		void erase(bucket_entry& entry) {
			assert(entry.is_occupied());
			occupied_count_--;
			entry.type_ = bucket_entry::type_t::deleted;
			entry.value.~pair();
			deleted_count_++;
			if constexpr (auto_rehash) compress();
		}
		// Delete by iterator
		template<bool auto_rehash = true>
		void erase(iterator i) {
			assert(i != end(*this));
			erase<auto_rehash>(i.entry());
		}
		// Deleting an item by index
		template<bool auto_rehash = true, typename KU>
		void erase(KU&& key) {
			sus::usize hash = hasher(key);
			sus::usize mask = capacity() - 1;
			sus::isize step = ((hash >> 16) | 1) & mask;
			if (!step) step = 1;
			for (sus::usize first = hash & mask, i = first;;) {
				bucket_entry* entry = &buckets_[i];
				switch (entry->type_) {
				#ifdef SUS_DEBUG
				case bucket_entry::type_t::empty: assert(false);
				#endif // !SUS_DEBUG
				case bucket_entry::type_t::occupied:
					if (entry->hash != hash || !pred(entry->value.first, key)) break;
					erase<auto_rehash>(*entry);
					return;
				case bucket_entry::type_t::deleted: break;
				}
				i = (i + step) & mask;
				if (i == first) break;
			}
		}
		// Searching for an item by key
		SUS_ATTRIB_PURE iterator found(const K& key) const {
			sus::usize hash = hasher_(key);
			sus::usize mask = capacity() - 1;
			sus::isize step = ((hash >> 16) | 1) & mask;
			if (!step) step = 1;
			for (sus::usize first = hash & mask, i = first;;) {
				bucket_entry* entry = &buckets_[i];
				switch (entry->type_) {
				case bucket_entry::type_t::empty: return end(*this);
				case bucket_entry::type_t::occupied:
					if (entry->hash != hash || !pred_(entry->value.first, key)) break;
					return iterator(*this, i);
				case bucket_entry::type_t::deleted: break;
				}
				i = (i + step) & mask;
				if (i == first) break;
			}
			return end(*this);
		}
		// Verify the existence of the key
		SUS_ATTRIB_PURE inline bool contains(const K& k) { return found(k) != end(*this); }
		// Insertion and replacement when found
		template<bool auto_replace = true, bool auto_rehash = true, typename KU, typename VU>
		inline iterator insert(KU&& key, VU&& value) {
			return do_insert_ex<auto_rehash, auto_replace>(sus::forward<KU>(key), [&](bucket_entry* entry) {
				::new(&entry->value.second) V(sus::forward<VU>(value));
				});
		}
		// Insertion and replacement when found
		template<bool auto_replace = true, bool auto_rehash = true, typename Pair>
		inline decltype(auto) insert(Pair&& value) {
			using KU = decltype(sus::forward<Pair>(value).first); using VU = decltype(sus::forward<Pair>(value).second);
			return insert_ex<auto_replace, auto_rehash>(sus::forward<KU>(sus::forward<Pair>(value).first), sus::forward<VU>(sus::forward<Pair>(value).second));
		}
		// Add all the elements of the hash table
		template<bool auto_replace = true, bool auto_rehash = true, sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, sus::iterator_value_t<iterator>>)
		inline void insert(const C& c) {
			for (auto i = begin(c); i != end(c); ++i) insert<auto_replace, auto_rehash>(*i);
		}
		// Insertion and replacement when found
		template<bool auto_replace = true, bool auto_rehash = true, typename KU, typename... Args>
		iterator emplace(KU&& key, Args&&... args) {
			return do_insert_ex<auto_replace, auto_rehash>(sus::forward<KU>(key), [&](bucket_entry* entry) {
				::new(&entry->value.second) V(sus::forward<Args>(args)...);
				});
		}
		// Accessing values by key
		template<bool auto_rehash = true, typename KU>
		inline V& operator[](KU&& k) { auto i = found(sus::forward<KU>(k)); if (i == end(*this)) i = emplace<false, auto_rehash>(sus::forward<KU>(k)); return i->second; }
		// Clean the hash table
		template<bool auto_rehash = true>
		inline void clear() {
			if (!occupied_count_ && !deleted_count_) return;
			for (auto i = data(*this), *last = data(*this) + capacity(); i != last; ++i) {
				if (i->is_occupied()) {
					i->value.~pair();
					i->type_ = bucket_entry::type_t::empty;
				}
				else if (i->is_deleted()) {
					i->type_ = bucket_entry::type_t::empty;
				}
			}
			occupied_count_ = 0;
			deleted_count_ = 0;
			if constexpr (auto_rehash) compress();
		}
	public:
		hashmap(const hashmap& other) : capacity_(other.capacity_), occupied_count_(0) {
			buckets_ = static_cast<bucket_entry*>(Alloc::malloc(other.capacity_ * sizeof(bucket_entry)));
			sus::memset(buckets_, 0, other.capacity_ * sizeof(bucket_entry));
			for (auto i = begin(other); i != end(other); ++i) insert<false, false>(i->first, i->second);
		}
		hashmap& operator=(const hashmap& other) {
			if (this != &other) {
				this->~hashmap();
				::new(this) hashmap(other);
			}
			return *this;
		}
		hashmap(hashmap&& other) : capacity_(other.capacity_), occupied_count_(other.occupied_count_), buckets_(other.buckets_) {
			other.buckets_ = nullptr; other.capacity_ = 0; other.occupied_count_ = 0;
		}
		hashmap& operator=(hashmap&& other) {
			if (this != &other) {
				this->~hashmap();
				::new(this) hashmap(sus::move(other));
			}
			return *this;
		}
		~hashmap() {
			if (buckets_) {
				clear();
				Alloc::free(buckets_);
			}
		}
		hashmap() {
			if constexpr (!Policy.lazy) {
				buckets_ = static_cast<bucket_entry*>(Alloc::malloc(base_capacity * sizeof(bucket_entry)));
				sus::memset(buckets_, 0, base_capacity * sizeof(bucket_entry));
				capacity_ = base_capacity;
			}
			else {
				capacity_ = 0;
				buckets_ = nullptr;
			}
		}
		hashmap(sus::usize capacity) : capacity_(capacity) {
			if (!sus::is_power2(capacity)) capacity_ = sus::round_to_power2(capacity);
			buckets_ = static_cast<bucket_entry*>(Alloc::malloc(capacity * sizeof(bucket_entry)));
			sus::memset(buckets_, 0, capacity * sizeof(bucket_entry));
		}
		template<sus::container_t C> requires(sus::is_same_v<sus::iterator_value_t<sus::iterator_t<C>>, sus::iterator_value_t<iterator>>)
		hashmap(const C& r) {
			if (!sus::is_power2(size(r))) capacity_ = sus::round_to_power2(size(r));
			else capacity_ = size(r);
			buckets_ = static_cast<bucket_entry*>(Alloc::malloc(capacity_ * sizeof(bucket_entry)));
			sus::memset(buckets_, 0, capacity_ * sizeof(bucket_entry));
			insert<false, false>(r);
		}
	};

	template<sus::container_t C> requires(sus::is_pair_v<sus::iterator_value_t<sus::iterator_t<C>>>)
	hashmap(const C&) -> hashmap<sus::iterator_value_t<sus::iterator_t<C>>::first_type, sus::iterator_value_t<sus::iterator_t<C>>::second_type>;

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_HASHMAP_ */