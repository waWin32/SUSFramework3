// susfwk-core-optimization_policy.hpp
//
/*
* =====---------------- optimization_policy.hpp - settings for specifying a task and optimizing it ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====-----------------------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_CORE_OPTIMIZATION_POLICY_
#define _SUSFWK_CORE_OPTIMIZATION_POLICY_

#include "susfwk/utils/core.hpp"

namespace sus {

	// -------------------------------------------------------------------

	// A single structure for specifying the purpose of using a container for optimizations
	struct optimization_policy {
		enum class data_volume_t {
			tiny,	// Temporary used (Optimizations for small volumes)
			medium,	// Common use (Less optimization for small volumes)
			large	// Large volumes (Without Optimizing for small volumes)
		} data_volume = data_volume_t::medium;
		enum class priority_t {
			memory,		// Saving memory at the expense of damage
			balanced,	// Balanced memory usage
			speed		// High memory consumption but higher performance
		} priority = priority_t::balanced;
		bool lazy = true; // They may not be used
	};

	// -------------------------------------------------------------------

}

#endif /* !_SUSFWK_CORE_OPTIMIZATION_POLICY_ */
