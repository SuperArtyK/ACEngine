
/** @file AEMisc/include/AETrigLookup.hpp
 *  This file contains the results for the trigonometric function lookup tables.
 *  These will be insanely fast, almost as the direct memory access (since just value in array), 
 *  but won't offer much of precision -- within 1 degree.
 *  (Un)Define ENGINE_ENABLE_MATH_LOOKUP flag in AEFlags.hpp to enable/disable this.
 *  
 *  @todo Add the ability to calculate the tables at compile time + control the size (36 elements or 3600, etc) Maybe it's a future (c++26) thing
 *
 *  Should not cause everything to break :)
 */

#pragma once

#ifndef ENGINE_TRIG_LOOKUP_HPP
#define ENGINE_TRIG_LOOKUP_HPP

#include "AEFlags.hpp"
#include "AEMath.hpp"
#include <cfloat>

#ifdef ENGINE_ENABLE_MATH_LOOKUP

/// A macro for the size of the trigonometric function lookup tables.
/// @note For now - do not touch!
#define AETRIG_TABLE_SIZE 360



namespace ace {
	
	namespace math {
		/// A lookup table for sin values
		constexpr float sinTable[AETRIG_TABLE_SIZE]{
			0.00000000f, 0.01745241f, 0.03489950f, 0.05233596f, 0.06975647f, 0.08715574f, 0.10452846f, 0.12186934f, 0.13917310f, 0.15643447f, 0.17364818f, 0.19080900f, 0.20791169f, 0.22495105f, 0.24192190f, 0.25881905f, 0.27563736f, 0.29237170f, 0.30901699f, 0.32556815f, 0.34202014f, 0.35836795f, 0.37460659f, 0.39073113f, 0.40673664f, 0.42261826f, 0.43837115f, 0.45399050f, 0.46947156f, 0.48480962f, 0.50000000f, 0.51503807f, 0.52991926f, 0.54463904f, 0.55919290f, 0.57357644f, 0.58778525f, 0.60181502f, 0.61566148f, 0.62932039f, 0.64278761f, 0.65605903f, 0.66913061f, 0.68199836f, 0.69465837f, 0.70710678f, 0.71933980f, 0.73135370f, 0.74314483f, 0.75470958f, 0.76604444f, 0.77714596f, 0.78801075f, 0.79863551f, 0.80901699f, 0.81915204f, 0.82903757f, 0.83867057f, 0.84804810f, 0.85716730f,
			0.86602540f, 0.87461971f, 0.88294759f, 0.89100652f, 0.89879405f, 0.90630779f, 0.91354546f, 0.92050485f, 0.92718385f, 0.93358043f, 0.93969262f, 0.94551858f, 0.95105652f, 0.95630476f, 0.96126170f, 0.96592583f, 0.97029573f, 0.97437006f, 0.97814760f, 0.98162718f, 0.98480775f, 0.98768834f, 0.99026807f, 0.99254615f, 0.99452190f, 0.99619470f, 0.99756405f, 0.99862953f, 0.99939083f, 0.99984770f, 1.00000000f, 0.99984770f, 0.99939083f, 0.99862953f, 0.99756405f, 0.99619470f, 0.99452190f, 0.99254615f, 0.99026807f, 0.98768834f, 0.98480775f, 0.98162718f, 0.97814760f, 0.97437006f, 0.97029573f, 0.96592583f, 0.96126170f, 0.95630476f, 0.95105652f, 0.94551858f, 0.93969262f, 0.93358043f, 0.92718385f, 0.92050485f, 0.91354546f, 0.90630779f, 0.89879405f, 0.89100652f, 0.88294759f, 0.87461971f,
			0.86602540f, 0.85716730f, 0.84804810f, 0.83867057f, 0.82903757f, 0.81915204f, 0.80901699f, 0.79863551f, 0.78801075f, 0.77714596f, 0.76604444f, 0.75470958f, 0.74314483f, 0.73135370f, 0.71933980f, 0.70710678f, 0.69465837f, 0.68199836f, 0.66913061f, 0.65605903f, 0.64278761f, 0.62932039f, 0.61566148f, 0.60181502f, 0.58778525f, 0.57357644f, 0.55919290f, 0.54463904f, 0.52991926f, 0.51503807f, 0.50000000f, 0.48480962f, 0.46947156f, 0.45399050f, 0.43837115f, 0.42261826f, 0.40673664f, 0.39073113f, 0.37460659f, 0.35836795f, 0.34202014f, 0.32556815f, 0.30901699f, 0.29237170f, 0.27563736f, 0.25881905f, 0.24192190f, 0.22495105f, 0.20791169f, 0.19080900f, 0.17364818f, 0.15643447f, 0.13917310f, 0.12186934f, 0.10452846f, 0.08715574f, 0.06975647f, 0.05233596f, 0.03489950f, 0.01745241f,
			0.00000000f, -0.01745241f, -0.03489950f, -0.05233596f, -0.06975647f, -0.08715574f, -0.10452846f, -0.12186934f, -0.13917310f, -0.15643447f, -0.17364818f, -0.19080900f, -0.20791169f, -0.22495105f, -0.24192190f, -0.25881905f, -0.27563736f, -0.29237170f, -0.30901699f, -0.32556815f, -0.34202014f, -0.35836795f, -0.37460659f, -0.39073113f, -0.40673664f, -0.42261826f, -0.43837115f, -0.45399050f, -0.46947156f, -0.48480962f, -0.50000000f, -0.51503807f, -0.52991926f, -0.54463904f, -0.55919290f, -0.57357644f, -0.58778525f, -0.60181502f, -0.61566148f, -0.62932039f, -0.64278761f, -0.65605903f, -0.66913061f, -0.68199836f, -0.69465837f, -0.70710678f, -0.71933980f, -0.73135370f, -0.74314483f, -0.75470958f, -0.76604444f, -0.77714596f, -0.78801075f, -0.79863551f, -0.80901699f, -0.81915204f, -0.82903757f, -0.83867057f, -0.84804810f, -0.85716730f,
			-0.86602540f, -0.87461971f, -0.88294759f, -0.89100652f, -0.89879405f, -0.90630779f, -0.91354546f, -0.92050485f, -0.92718385f, -0.93358043f, -0.93969262f, -0.94551858f, -0.95105652f, -0.95630476f, -0.96126170f, -0.96592583f, -0.97029573f, -0.97437006f, -0.97814760f, -0.98162718f, -0.98480775f, -0.98768834f, -0.99026807f, -0.99254615f, -0.99452190f, -0.99619470f, -0.99756405f, -0.99862953f, -0.99939083f, -0.99984770f, -1.00000000f, -0.99984770f, -0.99939083f, -0.99862953f, -0.99756405f, -0.99619470f, -0.99452190f, -0.99254615f, -0.99026807f, -0.98768834f, -0.98480775f, -0.98162718f, -0.97814760f, -0.97437006f, -0.97029573f, -0.96592583f, -0.96126170f, -0.95630476f, -0.95105652f, -0.94551858f, -0.93969262f, -0.93358043f, -0.92718385f, -0.92050485f, -0.91354546f, -0.90630779f, -0.89879405f, -0.89100652f, -0.88294759f, -0.87461971f,
			-0.86602540f, -0.85716730f, -0.84804810f, -0.83867057f, -0.82903757f, -0.81915204f, -0.80901699f, -0.79863551f, -0.78801075f, -0.77714596f, -0.76604444f, -0.75470958f, -0.74314483f, -0.73135370f, -0.71933980f, -0.70710678f, -0.69465837f, -0.68199836f, -0.66913061f, -0.65605903f, -0.64278761f, -0.62932039f, -0.61566148f, -0.60181502f, -0.58778525f, -0.57357644f, -0.55919290f, -0.54463904f, -0.52991926f, -0.51503807f, -0.50000000f, -0.48480962f, -0.46947156f, -0.45399050f, -0.43837115f, -0.42261826f, -0.40673664f, -0.39073113f, -0.37460659f, -0.35836795f, -0.34202014f, -0.32556815f, -0.30901699f, -0.29237170f, -0.27563736f, -0.25881905f, -0.24192190f, -0.22495105f, -0.20791169f, -0.19080900f, -0.17364818f, -0.15643447f, -0.13917310f, -0.12186934f, -0.10452846f, -0.08715574f, -0.06975647f, -0.05233596f, -0.03489950f, -0.01745241f,
		};
		/// A lookup table for cos values
		constexpr float cosTable[AETRIG_TABLE_SIZE]{
			1.00000000f, 0.99984770f, 0.99939083f, 0.99862953f, 0.99756405f, 0.99619470f, 0.99452190f, 0.99254615f, 0.99026807f, 0.98768834f, 0.98480775f, 0.98162718f, 0.97814760f, 0.97437006f, 0.97029573f, 0.96592583f, 0.96126170f, 0.95630476f, 0.95105652f, 0.94551858f, 0.93969262f, 0.93358043f, 0.92718385f, 0.92050485f, 0.91354546f, 0.90630779f, 0.89879405f, 0.89100652f, 0.88294759f, 0.87461971f, 0.86602540f, 0.85716730f, 0.84804810f, 0.83867057f, 0.82903757f, 0.81915204f, 0.80901699f, 0.79863551f, 0.78801075f, 0.77714596f, 0.76604444f, 0.75470958f, 0.74314483f, 0.73135370f, 0.71933980f, 0.70710678f, 0.69465837f, 0.68199836f, 0.66913061f, 0.65605903f, 0.64278761f, 0.62932039f, 0.61566148f, 0.60181502f, 0.58778525f, 0.57357644f, 0.55919290f, 0.54463904f, 0.52991926f, 0.51503807f,
			0.50000000f, 0.48480962f, 0.46947156f, 0.45399050f, 0.43837115f, 0.42261826f, 0.40673664f, 0.39073113f, 0.37460659f, 0.35836795f, 0.34202014f, 0.32556815f, 0.30901699f, 0.29237170f, 0.27563736f, 0.25881905f, 0.24192190f, 0.22495105f, 0.20791169f, 0.19080900f, 0.17364818f, 0.15643447f, 0.13917310f, 0.12186934f, 0.10452846f, 0.08715574f, 0.06975647f, 0.05233596f, 0.03489950f, 0.01745241f, 0.00000000f, -0.01745241f, -0.03489950f, -0.05233596f, -0.06975647f, -0.08715574f, -0.10452846f, -0.12186934f, -0.13917310f, -0.15643447f, -0.17364818f, -0.19080900f, -0.20791169f, -0.22495105f, -0.24192190f, -0.25881905f, -0.27563736f, -0.29237170f, -0.30901699f, -0.32556815f, -0.34202014f, -0.35836795f, -0.37460659f, -0.39073113f, -0.40673664f, -0.42261826f, -0.43837115f, -0.45399050f, -0.46947156f, -0.48480962f,
			-0.50000000f, -0.51503807f, -0.52991926f, -0.54463904f, -0.55919290f, -0.57357644f, -0.58778525f, -0.60181502f, -0.61566148f, -0.62932039f, -0.64278761f, -0.65605903f, -0.66913061f, -0.68199836f, -0.69465837f, -0.70710678f, -0.71933980f, -0.73135370f, -0.74314483f, -0.75470958f, -0.76604444f, -0.77714596f, -0.78801075f, -0.79863551f, -0.80901699f, -0.81915204f, -0.82903757f, -0.83867057f, -0.84804810f, -0.85716730f, -0.86602540f, -0.87461971f, -0.88294759f, -0.89100652f, -0.89879405f, -0.90630779f, -0.91354546f, -0.92050485f, -0.92718385f, -0.93358043f, -0.93969262f, -0.94551858f, -0.95105652f, -0.95630476f, -0.96126170f, -0.96592583f, -0.97029573f, -0.97437006f, -0.97814760f, -0.98162718f, -0.98480775f, -0.98768834f, -0.99026807f, -0.99254615f, -0.99452190f, -0.99619470f, -0.99756405f, -0.99862953f, -0.99939083f, -0.99984770f,
			-1.00000000f, -0.99984770f, -0.99939083f, -0.99862953f, -0.99756405f, -0.99619470f, -0.99452190f, -0.99254615f, -0.99026807f, -0.98768834f, -0.98480775f, -0.98162718f, -0.97814760f, -0.97437006f, -0.97029573f, -0.96592583f, -0.96126170f, -0.95630476f, -0.95105652f, -0.94551858f, -0.93969262f, -0.93358043f, -0.92718385f, -0.92050485f, -0.91354546f, -0.90630779f, -0.89879405f, -0.89100652f, -0.88294759f, -0.87461971f, -0.86602540f, -0.85716730f, -0.84804810f, -0.83867057f, -0.82903757f, -0.81915204f, -0.80901699f, -0.79863551f, -0.78801075f, -0.77714596f, -0.76604444f, -0.75470958f, -0.74314483f, -0.73135370f, -0.71933980f, -0.70710678f, -0.69465837f, -0.68199836f, -0.66913061f, -0.65605903f, -0.64278761f, -0.62932039f, -0.61566148f, -0.60181502f, -0.58778525f, -0.57357644f, -0.55919290f, -0.54463904f, -0.52991926f, -0.51503807f,
			-0.50000000f, -0.48480962f, -0.46947156f, -0.45399050f, -0.43837115f, -0.42261826f, -0.40673664f, -0.39073113f, -0.37460659f, -0.35836795f, -0.34202014f, -0.32556815f, -0.30901699f, -0.29237170f, -0.27563736f, -0.25881905f, -0.24192190f, -0.22495105f, -0.20791169f, -0.19080900f, -0.17364818f, -0.15643447f, -0.13917310f, -0.12186934f, -0.10452846f, -0.08715574f, -0.06975647f, -0.05233596f, -0.03489950f, -0.01745241f, -0.00000000f, 0.01745241f, 0.03489950f, 0.05233596f, 0.06975647f, 0.08715574f, 0.10452846f, 0.12186934f, 0.13917310f, 0.15643447f, 0.17364818f, 0.19080900f, 0.20791169f, 0.22495105f, 0.24192190f, 0.25881905f, 0.27563736f, 0.29237170f, 0.30901699f, 0.32556815f, 0.34202014f, 0.35836795f, 0.37460659f, 0.39073113f, 0.40673664f, 0.42261826f, 0.43837115f, 0.45399050f, 0.46947156f, 0.48480962f,
			0.50000000f, 0.51503807f, 0.52991926f, 0.54463904f, 0.55919290f, 0.57357644f, 0.58778525f, 0.60181502f, 0.61566148f, 0.62932039f, 0.64278761f, 0.65605903f, 0.66913061f, 0.68199836f, 0.69465837f, 0.70710678f, 0.71933980f, 0.73135370f, 0.74314483f, 0.75470958f, 0.76604444f, 0.77714596f, 0.78801075f, 0.79863551f, 0.80901699f, 0.81915204f, 0.82903757f, 0.83867057f, 0.84804810f, 0.85716730f, 0.86602540f, 0.87461971f, 0.88294759f, 0.89100652f, 0.89879405f, 0.90630779f, 0.91354546f, 0.92050485f, 0.92718385f, 0.93358043f, 0.93969262f, 0.94551858f, 0.95105652f, 0.95630476f, 0.96126170f, 0.96592583f, 0.97029573f, 0.97437006f, 0.97814760f, 0.98162718f, 0.98480775f, 0.98768834f, 0.99026807f, 0.99254615f, 0.99452190f, 0.99619470f, 0.99756405f, 0.99862953f, 0.99939083f, 0.99984770f,
		};
		/// A lookup table for tan values
		constexpr float tanTable[AETRIG_TABLE_SIZE]{
			0.00000000f, 0.01745506f, 0.03492077f, 0.05240778f, 0.06992681f, 0.08748866f, 0.10510424f, 0.12278456f, 0.14054083f, 0.15838444f, 0.17632698f, 0.19438031f, 0.21255656f, 0.23086819f, 0.24932800f, 0.26794919f, 0.28674539f, 0.30573068f, 0.32491970f, 0.34432761f, 0.36397023f, 0.38386404f, 0.40402623f, 0.42447482f, 0.44522869f, 0.46630766f, 0.48773259f, 0.50952545f, 0.53170943f, 0.55430905f, 0.57735027f, 0.60086062f, 0.62486935f, 0.64940759f, 0.67450852f, 0.70020754f, 0.72654253f, 0.75355405f, 0.78128563f, 0.80978403f, 0.83909963f, 0.86928674f, 0.90040404f, 0.93251509f, 0.96568877f, 1.00000000f, 1.03553031f, 1.07236871f, 1.11061251f, 1.15036841f, 1.19175359f, 1.23489716f, 1.27994163f, 1.32704482f, 1.37638192f, 1.42814801f, 1.48256097f, 1.53986496f, 1.60033453f, 1.66427948f,
			1.73205081f, 1.80404776f, 1.88072647f, 1.96261051f, 2.05030384f, 2.14450692f, 2.24603677f, 2.35585237f, 2.47508685f, 2.60508906f, 2.74747742f, 2.90421088f, 3.07768354f, 3.27085262f, 3.48741444f, 3.73205081f, 4.01078093f, 4.33147587f, 4.70463011f, 5.14455402f, 5.67128182f, 6.31375151f, 7.11536972f, 8.14434643f, 9.51436445f, 11.43005230f, 14.30066626f, 19.08113669f, 28.63625328f, 57.28996163f, FLT_MAX, -57.28996163f, -28.63625328f, -19.08113669f, -14.30066626f, -11.43005230f, -9.51436445f, -8.14434643f, -7.11536972f, -6.31375151f, -5.67128182f, -5.14455402f, -4.70463011f, -4.33147587f, -4.01078093f, -3.73205081f, -3.48741444f, -3.27085262f, -3.07768354f, -2.90421088f, -2.74747742f, -2.60508906f, -2.47508685f, -2.35585237f, -2.24603677f, -2.14450692f, -2.05030384f, -1.96261051f, -1.88072647f, -1.80404776f,
			-1.73205081f, -1.66427948f, -1.60033453f, -1.53986496f, -1.48256097f, -1.42814801f, -1.37638192f, -1.32704482f, -1.27994163f, -1.23489716f, -1.19175359f, -1.15036841f, -1.11061251f, -1.07236871f, -1.03553031f, -1.00000000f, -0.96568877f, -0.93251509f, -0.90040404f, -0.86928674f, -0.83909963f, -0.80978403f, -0.78128563f, -0.75355405f, -0.72654253f, -0.70020754f, -0.67450852f, -0.64940759f, -0.62486935f, -0.60086062f, -0.57735027f, -0.55430905f, -0.53170943f, -0.50952545f, -0.48773259f, -0.46630766f, -0.44522869f, -0.42447482f, -0.40402623f, -0.38386404f, -0.36397023f, -0.34432761f, -0.32491970f, -0.30573068f, -0.28674539f, -0.26794919f, -0.24932800f, -0.23086819f, -0.21255656f, -0.19438031f, -0.17632698f, -0.15838444f, -0.14054083f, -0.12278456f, -0.10510424f, -0.08748866f, -0.06992681f, -0.05240778f, -0.03492077f, -0.01745506f,
			-0.00000000f, 0.01745506f, 0.03492077f, 0.05240778f, 0.06992681f, 0.08748866f, 0.10510424f, 0.12278456f, 0.14054083f, 0.15838444f, 0.17632698f, 0.19438031f, 0.21255656f, 0.23086819f, 0.24932800f, 0.26794919f, 0.28674539f, 0.30573068f, 0.32491970f, 0.34432761f, 0.36397023f, 0.38386404f, 0.40402623f, 0.42447482f, 0.44522869f, 0.46630766f, 0.48773259f, 0.50952545f, 0.53170943f, 0.55430905f, 0.57735027f, 0.60086062f, 0.62486935f, 0.64940759f, 0.67450852f, 0.70020754f, 0.72654253f, 0.75355405f, 0.78128563f, 0.80978403f, 0.83909963f, 0.86928674f, 0.90040404f, 0.93251509f, 0.96568877f, 1.00000000f, 1.03553031f, 1.07236871f, 1.11061251f, 1.15036841f, 1.19175359f, 1.23489716f, 1.27994163f, 1.32704482f, 1.37638192f, 1.42814801f, 1.48256097f, 1.53986496f, 1.60033453f, 1.66427948f,
			1.73205081f, 1.80404776f, 1.88072647f, 1.96261051f, 2.05030384f, 2.14450692f, 2.24603677f, 2.35585237f, 2.47508685f, 2.60508906f, 2.74747742f, 2.90421088f, 3.07768354f, 3.27085262f, 3.48741444f, 3.73205081f, 4.01078093f, 4.33147587f, 4.70463011f, 5.14455402f, 5.67128182f, 6.31375151f, 7.11536972f, 8.14434643f, 9.51436445f, 11.43005230f, 14.30066626f, 19.08113669f, 28.63625328f, 57.28996163f, FLT_MAX, -57.28996163f, -28.63625328f, -19.08113669f, -14.30066626f, -11.43005230f, -9.51436445f, -8.14434643f, -7.11536972f, -6.31375151f, -5.67128182f, -5.14455402f, -4.70463011f, -4.33147587f, -4.01078093f, -3.73205081f, -3.48741444f, -3.27085262f, -3.07768354f, -2.90421088f, -2.74747742f, -2.60508906f, -2.47508685f, -2.35585237f, -2.24603677f, -2.14450692f, -2.05030384f, -1.96261051f, -1.88072647f, -1.80404776f,
			-1.73205081f, -1.66427948f, -1.60033453f, -1.53986496f, -1.48256097f, -1.42814801f, -1.37638192f, -1.32704482f, -1.27994163f, -1.23489716f, -1.19175359f, -1.15036841f, -1.11061251f, -1.07236871f, -1.03553031f, -1.00000000f, -0.96568877f, -0.93251509f, -0.90040404f, -0.86928674f, -0.83909963f, -0.80978403f, -0.78128563f, -0.75355405f, -0.72654253f, -0.70020754f, -0.67450852f, -0.64940759f, -0.62486935f, -0.60086062f, -0.57735027f, -0.55430905f, -0.53170943f, -0.50952545f, -0.48773259f, -0.46630766f, -0.44522869f, -0.42447482f, -0.40402623f, -0.38386404f, -0.36397023f, -0.34432761f, -0.32491970f, -0.30573068f, -0.28674539f, -0.26794919f, -0.24932800f, -0.23086819f, -0.21255656f, -0.19438031f, -0.17632698f, -0.15838444f, -0.14054083f, -0.12278456f, -0.10510424f, -0.08748866f, -0.06992681f, -0.05240778f, -0.03492077f, -0.01745506f,
		};

		/// A lookup table for cot values
		constexpr float cotTable[AETRIG_TABLE_SIZE]{
			FLT_MAX, 57.28996163f, 28.63625328f, 19.08113669f, 14.30066626f, 11.43005230f, 9.51436445f, 8.14434643f, 7.11536972f, 6.31375151f, 5.67128182f, 5.14455402f, 4.70463011f, 4.33147587f, 4.01078093f, 3.73205081f, 3.48741444f, 3.27085262f, 3.07768354f, 2.90421088f, 2.74747742f, 2.60508906f, 2.47508685f, 2.35585237f, 2.24603677f, 2.14450692f, 2.05030384f, 1.96261051f, 1.88072647f, 1.80404776f, 1.73205081f, 1.66427948f, 1.60033453f, 1.53986496f, 1.48256097f, 1.42814801f, 1.37638192f, 1.32704482f, 1.27994163f, 1.23489716f, 1.19175359f, 1.15036841f, 1.11061251f, 1.07236871f, 1.03553031f, 1.00000000f, 0.96568877f, 0.93251509f, 0.90040404f, 0.86928674f, 0.83909963f, 0.80978403f, 0.78128563f, 0.75355405f, 0.72654253f, 0.70020754f, 0.67450852f, 0.64940759f, 0.62486935f, 0.60086062f,
			0.57735027f, 0.55430905f, 0.53170943f, 0.50952545f, 0.48773259f, 0.46630766f, 0.44522869f, 0.42447482f, 0.40402623f, 0.38386404f, 0.36397023f, 0.34432761f, 0.32491970f, 0.30573068f, 0.28674539f, 0.26794919f, 0.24932800f, 0.23086819f, 0.21255656f, 0.19438031f, 0.17632698f, 0.15838444f, 0.14054083f, 0.12278456f, 0.10510424f, 0.08748866f, 0.06992681f, 0.05240778f, 0.03492077f, 0.01745506f, 0.00000000f, -0.01745506f, -0.03492077f, -0.05240778f, -0.06992681f, -0.08748866f, -0.10510424f, -0.12278456f, -0.14054083f, -0.15838444f, -0.17632698f, -0.19438031f, -0.21255656f, -0.23086819f, -0.24932800f, -0.26794919f, -0.28674539f, -0.30573068f, -0.32491970f, -0.34432761f, -0.36397023f, -0.38386404f, -0.40402623f, -0.42447482f, -0.44522869f, -0.46630766f, -0.48773259f, -0.50952545f, -0.53170943f, -0.55430905f,
			-0.57735027f, -0.60086062f, -0.62486935f, -0.64940759f, -0.67450852f, -0.70020754f, -0.72654253f, -0.75355405f, -0.78128563f, -0.80978403f, -0.83909963f, -0.86928674f, -0.90040404f, -0.93251509f, -0.96568877f, -1.00000000f, -1.03553031f, -1.07236871f, -1.11061251f, -1.15036841f, -1.19175359f, -1.23489716f, -1.27994163f, -1.32704482f, -1.37638192f, -1.42814801f, -1.48256097f, -1.53986496f, -1.60033453f, -1.66427948f, -1.73205081f, -1.80404776f, -1.88072647f, -1.96261051f, -2.05030384f, -2.14450692f, -2.24603677f, -2.35585237f, -2.47508685f, -2.60508906f, -2.74747742f, -2.90421088f, -3.07768354f, -3.27085262f, -3.48741444f, -3.73205081f, -4.01078093f, -4.33147587f, -4.70463011f, -5.14455402f, -5.67128182f, -6.31375151f, -7.11536972f, -8.14434643f, -9.51436445f, -11.43005230f, -14.30066626f, -19.08113669f, -28.63625328f, -57.28996163f,
			-FLT_MAX, 57.28996163f, 28.63625328f, 19.08113669f, 14.30066626f, 11.43005230f, 9.51436445f, 8.14434643f, 7.11536972f, 6.31375151f, 5.67128182f, 5.14455402f, 4.70463011f, 4.33147587f, 4.01078093f, 3.73205081f, 3.48741444f, 3.27085262f, 3.07768354f, 2.90421088f, 2.74747742f, 2.60508906f, 2.47508685f, 2.35585237f, 2.24603677f, 2.14450692f, 2.05030384f, 1.96261051f, 1.88072647f, 1.80404776f, 1.73205081f, 1.66427948f, 1.60033453f, 1.53986496f, 1.48256097f, 1.42814801f, 1.37638192f, 1.32704482f, 1.27994163f, 1.23489716f, 1.19175359f, 1.15036841f, 1.11061251f, 1.07236871f, 1.03553031f, 1.00000000f, 0.96568877f, 0.93251509f, 0.90040404f, 0.86928674f, 0.83909963f, 0.80978403f, 0.78128563f, 0.75355405f, 0.72654253f, 0.70020754f, 0.67450852f, 0.64940759f, 0.62486935f, 0.60086062f,
			0.57735027f, 0.55430905f, 0.53170943f, 0.50952545f, 0.48773259f, 0.46630766f, 0.44522869f, 0.42447482f, 0.40402623f, 0.38386404f, 0.36397023f, 0.34432761f, 0.32491970f, 0.30573068f, 0.28674539f, 0.26794919f, 0.24932800f, 0.23086819f, 0.21255656f, 0.19438031f, 0.17632698f, 0.15838444f, 0.14054083f, 0.12278456f, 0.10510424f, 0.08748866f, 0.06992681f, 0.05240778f, 0.03492077f, 0.01745506f, 0.00000000f, -0.01745506f, -0.03492077f, -0.05240778f, -0.06992681f, -0.08748866f, -0.10510424f, -0.12278456f, -0.14054083f, -0.15838444f, -0.17632698f, -0.19438031f, -0.21255656f, -0.23086819f, -0.24932800f, -0.26794919f, -0.28674539f, -0.30573068f, -0.32491970f, -0.34432761f, -0.36397023f, -0.38386404f, -0.40402623f, -0.42447482f, -0.44522869f, -0.46630766f, -0.48773259f, -0.50952545f, -0.53170943f, -0.55430905f,
			-0.57735027f, -0.60086062f, -0.62486935f, -0.64940759f, -0.67450852f, -0.70020754f, -0.72654253f, -0.75355405f, -0.78128563f, -0.80978403f, -0.83909963f, -0.86928674f, -0.90040404f, -0.93251509f, -0.96568877f, -1.00000000f, -1.03553031f, -1.07236871f, -1.11061251f, -1.15036841f, -1.19175359f, -1.23489716f, -1.27994163f, -1.32704482f, -1.37638192f, -1.42814801f, -1.48256097f, -1.53986496f, -1.60033453f, -1.66427948f, -1.73205081f, -1.80404776f, -1.88072647f, -1.96261051f, -2.05030384f, -2.14450692f, -2.24603677f, -2.35585237f, -2.47508685f, -2.60508906f, -2.74747742f, -2.90421088f, -3.07768354f, -3.27085262f, -3.48741444f, -3.73205081f, -4.01078093f, -4.33147587f, -4.70463011f, -5.14455402f, -5.67128182f, -6.31375151f, -7.11536972f, -8.14434643f, -9.51436445f, -11.43005230f, -14.30066626f, -19.08113669f, -28.63625328f, -57.28996163f,
		};

		/// A lookup table for csc values
		constexpr float cscTable[AETRIG_TABLE_SIZE]{
			FLT_MAX, 57.29868850f, 28.65370835f, 19.10732261f, 14.33558703f, 11.47371325f, 9.56677223f, 8.20550905f, 7.18529653f, 6.39245322f, 5.75877048f, 5.24084306f, 4.80973434f, 4.44541148f, 4.13356549f, 3.86370331f, 3.62795528f, 3.42030362f, 3.23606798f, 3.07155349f, 2.92380440f, 2.79042811f, 2.66946716f, 2.55930467f, 2.45859334f, 2.36620158f, 2.28117203f, 2.20268926f, 2.13005447f, 2.06266534f, 2.00000000f, 1.94160403f, 1.88707991f, 1.83607846f, 1.78829165f, 1.74344680f, 1.70130162f, 1.66164014f, 1.62426925f, 1.58901573f, 1.55572383f, 1.52425309f, 1.49447655f, 1.46627919f, 1.43955654f, 1.41421356f, 1.39016359f, 1.36732746f, 1.34563273f, 1.32501299f, 1.30540729f, 1.28675957f, 1.26901822f, 1.25213566f, 1.23606798f, 1.22077459f, 1.20621795f, 1.19236329f, 1.17917840f, 1.16663340f,
			1.15470054f, 1.14335407f, 1.13257005f, 1.12232624f, 1.11260194f, 1.10337792f, 1.09463628f, 1.08636038f, 1.07853474f, 1.07114499f, 1.06417777f, 1.05762068f, 1.05146222f, 1.04569176f, 1.04029944f, 1.03527618f, 1.03061363f, 1.02630411f, 1.02234059f, 1.01871669f, 1.01542661f, 1.01246513f, 1.00982757f, 1.00750983f, 1.00550828f, 1.00381984f, 1.00244190f, 1.00137235f, 1.00060954f, 1.00015233f, 1.00000000f, 1.00015233f, 1.00060954f, 1.00137235f, 1.00244190f, 1.00381984f, 1.00550828f, 1.00750983f, 1.00982757f, 1.01246513f, 1.01542661f, 1.01871669f, 1.02234059f, 1.02630411f, 1.03061363f, 1.03527618f, 1.04029944f, 1.04569176f, 1.05146222f, 1.05762068f, 1.06417777f, 1.07114499f, 1.07853474f, 1.08636038f, 1.09463628f, 1.10337792f, 1.11260194f, 1.12232624f, 1.13257005f, 1.14335407f,
			1.15470054f, 1.16663340f, 1.17917840f, 1.19236329f, 1.20621795f, 1.22077459f, 1.23606798f, 1.25213566f, 1.26901822f, 1.28675957f, 1.30540729f, 1.32501299f, 1.34563273f, 1.36732746f, 1.39016359f, 1.41421356f, 1.43955654f, 1.46627919f, 1.49447655f, 1.52425309f, 1.55572383f, 1.58901573f, 1.62426925f, 1.66164014f, 1.70130162f, 1.74344680f, 1.78829165f, 1.83607846f, 1.88707991f, 1.94160403f, 2.00000000f, 2.06266534f, 2.13005447f, 2.20268926f, 2.28117203f, 2.36620158f, 2.45859334f, 2.55930467f, 2.66946716f, 2.79042811f, 2.92380440f, 3.07155349f, 3.23606798f, 3.42030362f, 3.62795528f, 3.86370331f, 4.13356549f, 4.44541148f, 4.80973434f, 5.24084306f, 5.75877048f, 6.39245322f, 7.18529653f, 8.20550905f, 9.56677223f, 11.47371325f, 14.33558703f, 19.10732261f, 28.65370835f, 57.29868850f,
			-FLT_MAX, -57.29868850f, -28.65370835f, -19.10732261f, -14.33558703f, -11.47371325f, -9.56677223f, -8.20550905f, -7.18529653f, -6.39245322f, -5.75877048f, -5.24084306f, -4.80973434f, -4.44541148f, -4.13356549f, -3.86370331f, -3.62795528f, -3.42030362f, -3.23606798f, -3.07155349f, -2.92380440f, -2.79042811f, -2.66946716f, -2.55930467f, -2.45859334f, -2.36620158f, -2.28117203f, -2.20268926f, -2.13005447f, -2.06266534f, -2.00000000f, -1.94160403f, -1.88707991f, -1.83607846f, -1.78829165f, -1.74344680f, -1.70130162f, -1.66164014f, -1.62426925f, -1.58901573f, -1.55572383f, -1.52425309f, -1.49447655f, -1.46627919f, -1.43955654f, -1.41421356f, -1.39016359f, -1.36732746f, -1.34563273f, -1.32501299f, -1.30540729f, -1.28675957f, -1.26901822f, -1.25213566f, -1.23606798f, -1.22077459f, -1.20621795f, -1.19236329f, -1.17917840f, -1.16663340f,
			-1.15470054f, -1.14335407f, -1.13257005f, -1.12232624f, -1.11260194f, -1.10337792f, -1.09463628f, -1.08636038f, -1.07853474f, -1.07114499f, -1.06417777f, -1.05762068f, -1.05146222f, -1.04569176f, -1.04029944f, -1.03527618f, -1.03061363f, -1.02630411f, -1.02234059f, -1.01871669f, -1.01542661f, -1.01246513f, -1.00982757f, -1.00750983f, -1.00550828f, -1.00381984f, -1.00244190f, -1.00137235f, -1.00060954f, -1.00015233f, -1.00000000f, -1.00015233f, -1.00060954f, -1.00137235f, -1.00244190f, -1.00381984f, -1.00550828f, -1.00750983f, -1.00982757f, -1.01246513f, -1.01542661f, -1.01871669f, -1.02234059f, -1.02630411f, -1.03061363f, -1.03527618f, -1.04029944f, -1.04569176f, -1.05146222f, -1.05762068f, -1.06417777f, -1.07114499f, -1.07853474f, -1.08636038f, -1.09463628f, -1.10337792f, -1.11260194f, -1.12232624f, -1.13257005f, -1.14335407f,
			-1.15470054f, -1.16663340f, -1.17917840f, -1.19236329f, -1.20621795f, -1.22077459f, -1.23606798f, -1.25213566f, -1.26901822f, -1.28675957f, -1.30540729f, -1.32501299f, -1.34563273f, -1.36732746f, -1.39016359f, -1.41421356f, -1.43955654f, -1.46627919f, -1.49447655f, -1.52425309f, -1.55572383f, -1.58901573f, -1.62426925f, -1.66164014f, -1.70130162f, -1.74344680f, -1.78829165f, -1.83607846f, -1.88707991f, -1.94160403f, -2.00000000f, -2.06266534f, -2.13005447f, -2.20268926f, -2.28117203f, -2.36620158f, -2.45859334f, -2.55930467f, -2.66946716f, -2.79042811f, -2.92380440f, -3.07155349f, -3.23606798f, -3.42030362f, -3.62795528f, -3.86370331f, -4.13356549f, -4.44541148f, -4.80973434f, -5.24084306f, -5.75877048f, -6.39245322f, -7.18529653f, -8.20550905f, -9.56677223f, -11.47371325f, -14.33558703f, -19.10732261f, -28.65370835f, -57.29868850f,
		};

		/// A lookup table for sec values
		constexpr float secTable[AETRIG_TABLE_SIZE]{
			1.00000000f, 1.00015233f, 1.00060954f, 1.00137235f, 1.00244190f, 1.00381984f, 1.00550828f, 1.00750983f, 1.00982757f, 1.01246513f, 1.01542661f, 1.01871669f, 1.02234059f, 1.02630411f, 1.03061363f, 1.03527618f, 1.04029944f, 1.04569176f, 1.05146222f, 1.05762068f, 1.06417777f, 1.07114499f, 1.07853474f, 1.08636038f, 1.09463628f, 1.10337792f, 1.11260194f, 1.12232624f, 1.13257005f, 1.14335407f, 1.15470054f, 1.16663340f, 1.17917840f, 1.19236329f, 1.20621795f, 1.22077459f, 1.23606798f, 1.25213566f, 1.26901822f, 1.28675957f, 1.30540729f, 1.32501299f, 1.34563273f, 1.36732746f, 1.39016359f, 1.41421356f, 1.43955654f, 1.46627919f, 1.49447655f, 1.52425309f, 1.55572383f, 1.58901573f, 1.62426925f, 1.66164014f, 1.70130162f, 1.74344680f, 1.78829165f, 1.83607846f, 1.88707991f, 1.94160403f,
			2.00000000f, 2.06266534f, 2.13005447f, 2.20268926f, 2.28117203f, 2.36620158f, 2.45859334f, 2.55930467f, 2.66946716f, 2.79042811f, 2.92380440f, 3.07155349f, 3.23606798f, 3.42030362f, 3.62795528f, 3.86370331f, 4.13356549f, 4.44541148f, 4.80973434f, 5.24084306f, 5.75877048f, 6.39245322f, 7.18529653f, 8.20550905f, 9.56677223f, 11.47371325f, 14.33558703f, 19.10732261f, 28.65370835f, 57.29868850f, FLT_MAX, -57.29868850f, -28.65370835f, -19.10732261f, -14.33558703f, -11.47371325f, -9.56677223f, -8.20550905f, -7.18529653f, -6.39245322f, -5.75877048f, -5.24084306f, -4.80973434f, -4.44541148f, -4.13356549f, -3.86370331f, -3.62795528f, -3.42030362f, -3.23606798f, -3.07155349f, -2.92380440f, -2.79042811f, -2.66946716f, -2.55930467f, -2.45859334f, -2.36620158f, -2.28117203f, -2.20268926f, -2.13005447f, -2.06266534f,
			-2.00000000f, -1.94160403f, -1.88707991f, -1.83607846f, -1.78829165f, -1.74344680f, -1.70130162f, -1.66164014f, -1.62426925f, -1.58901573f, -1.55572383f, -1.52425309f, -1.49447655f, -1.46627919f, -1.43955654f, -1.41421356f, -1.39016359f, -1.36732746f, -1.34563273f, -1.32501299f, -1.30540729f, -1.28675957f, -1.26901822f, -1.25213566f, -1.23606798f, -1.22077459f, -1.20621795f, -1.19236329f, -1.17917840f, -1.16663340f, -1.15470054f, -1.14335407f, -1.13257005f, -1.12232624f, -1.11260194f, -1.10337792f, -1.09463628f, -1.08636038f, -1.07853474f, -1.07114499f, -1.06417777f, -1.05762068f, -1.05146222f, -1.04569176f, -1.04029944f, -1.03527618f, -1.03061363f, -1.02630411f, -1.02234059f, -1.01871669f, -1.01542661f, -1.01246513f, -1.00982757f, -1.00750983f, -1.00550828f, -1.00381984f, -1.00244190f, -1.00137235f, -1.00060954f, -1.00015233f,
			-1.00000000f, -1.00015233f, -1.00060954f, -1.00137235f, -1.00244190f, -1.00381984f, -1.00550828f, -1.00750983f, -1.00982757f, -1.01246513f, -1.01542661f, -1.01871669f, -1.02234059f, -1.02630411f, -1.03061363f, -1.03527618f, -1.04029944f, -1.04569176f, -1.05146222f, -1.05762068f, -1.06417777f, -1.07114499f, -1.07853474f, -1.08636038f, -1.09463628f, -1.10337792f, -1.11260194f, -1.12232624f, -1.13257005f, -1.14335407f, -1.15470054f, -1.16663340f, -1.17917840f, -1.19236329f, -1.20621795f, -1.22077459f, -1.23606798f, -1.25213566f, -1.26901822f, -1.28675957f, -1.30540729f, -1.32501299f, -1.34563273f, -1.36732746f, -1.39016359f, -1.41421356f, -1.43955654f, -1.46627919f, -1.49447655f, -1.52425309f, -1.55572383f, -1.58901573f, -1.62426925f, -1.66164014f, -1.70130162f, -1.74344680f, -1.78829165f, -1.83607846f, -1.88707991f, -1.94160403f,
			-2.00000000f, -2.06266534f, -2.13005447f, -2.20268926f, -2.28117203f, -2.36620158f, -2.45859334f, -2.55930467f, -2.66946716f, -2.79042811f, -2.92380440f, -3.07155349f, -3.23606798f, -3.42030362f, -3.62795528f, -3.86370331f, -4.13356549f, -4.44541148f, -4.80973434f, -5.24084306f, -5.75877048f, -6.39245322f, -7.18529653f, -8.20550905f, -9.56677223f, -11.47371325f, -14.33558703f, -19.10732261f, -28.65370835f, -57.29868850f, -FLT_MAX, 57.29868850f, 28.65370835f, 19.10732261f, 14.33558703f, 11.47371325f, 9.56677223f, 8.20550905f, 7.18529653f, 6.39245322f, 5.75877048f, 5.24084306f, 4.80973434f, 4.44541148f, 4.13356549f, 3.86370331f, 3.62795528f, 3.42030362f, 3.23606798f, 3.07155349f, 2.92380440f, 2.79042811f, 2.66946716f, 2.55930467f, 2.45859334f, 2.36620158f, 2.28117203f, 2.20268926f, 2.13005447f, 2.06266534f,
			2.00000000f, 1.94160403f, 1.88707991f, 1.83607846f, 1.78829165f, 1.74344680f, 1.70130162f, 1.66164014f, 1.62426925f, 1.58901573f, 1.55572383f, 1.52425309f, 1.49447655f, 1.46627919f, 1.43955654f, 1.41421356f, 1.39016359f, 1.36732746f, 1.34563273f, 1.32501299f, 1.30540729f, 1.28675957f, 1.26901822f, 1.25213566f, 1.23606798f, 1.22077459f, 1.20621795f, 1.19236329f, 1.17917840f, 1.16663340f, 1.15470054f, 1.14335407f, 1.13257005f, 1.12232624f, 1.11260194f, 1.10337792f, 1.09463628f, 1.08636038f, 1.07853474f, 1.07114499f, 1.06417777f, 1.05762068f, 1.05146222f, 1.04569176f, 1.04029944f, 1.03527618f, 1.03061363f, 1.02630411f, 1.02234059f, 1.01871669f, 1.01542661f, 1.01246513f, 1.00982757f, 1.00750983f, 1.00550828f, 1.00381984f, 1.00244190f, 1.00137235f, 1.00060954f, 1.00015233f,
		};
		
		/// <summary>
		/// Returns the index to the trig table from the given (int) degree number
		/// </summary>
		/// <param name="a">The integer degree number</param>
		/// <returns>The legal index to the lookup tables</returns>
		constexpr std::size_t _getTrigTableIndex(const llint a) noexcept {
			return (a < 0) ? (AETRIG_TABLE_SIZE + (a % AETRIG_TABLE_SIZE)) : (a % AETRIG_TABLE_SIZE);
		}

		/// <summary>
		/// Rounds the degrees value, and looks up the value of sine in the table.
		/// </summary>
		/// <param name="degrees">The value of degrees to find a sine from</param>
		/// <returns>Float sine lookup table value that matches the (rounded) degrees amount</returns>
		constexpr float sinDeg_table(const float degrees) noexcept {
			return sinTable[ace::math::_getTrigTableIndex( ace::math::roundToInt<llint>(degrees) )];
		}

		/// <summary>
		/// Rounds the degrees value, and looks up the value of cosine in the table.
		/// </summary>
		/// <param name="degrees">The value of degrees to find a cosine from</param>
		/// <returns>Float cosine lookup table value that matches the (rounded) degrees amount</returns>
		constexpr float cosDeg_table(const float degrees) noexcept {
			return cosTable[ace::math::_getTrigTableIndex(ace::math::roundToInt<llint>(degrees))];
		}

		/// <summary>
		/// Rounds the degrees value, and looks up the value of tangent in the table.
		/// </summary>
		/// <param name="degrees">The value of degrees to find a tangent from</param>
		/// <returns>Float tangent lookup table value that matches the (rounded) degrees amount</returns>
		constexpr float tanDeg_table(const float degrees) noexcept {
			return tanTable[ace::math::_getTrigTableIndex(ace::math::roundToInt<llint>(degrees))];
		}

		/// <summary>
		/// Rounds the degrees value, and looks up the value of cotangent in the table.
		/// </summary>
		/// <param name="degrees">The value of degrees to find a cotangent from</param>
		/// <returns>Float cotangent lookup table value that matches the (rounded) degrees amount</returns>
		constexpr float cotDeg_table(const float degrees) noexcept {
			return cotTable[ace::math::_getTrigTableIndex(ace::math::roundToInt<llint>(degrees))];
		}

		/// <summary>
		/// Rounds the degrees value, and looks up the value of cosecant in the table.
		/// </summary>
		/// <param name="degrees">The value of degrees to find a cosecant from</param>
		/// <returns>Float cosecant lookup table value that matches the (rounded) degrees amount</returns>
		constexpr float cscDeg_table(const float degrees) noexcept {
			return cscTable[ace::math::_getTrigTableIndex(ace::math::roundToInt<llint>(degrees))];
		}

		/// <summary>
		/// Rounds the degrees value, and looks up the value of secant in the table.
		/// </summary>
		/// <param name="degrees">The value of degrees to find a secant from</param>
		/// <returns>Float secant lookup table value that matches the (rounded) degrees amount</returns>
		constexpr float secDeg_table(const float degrees) noexcept {
			return secTable[ace::math::_getTrigTableIndex(ace::math::roundToInt<llint>(degrees))];
		}

		/// <summary>
		/// Calculates the interpolated sine value from the sine lookup table
		/// @note Correct to the sine value up to a few decimal point digits: truncating digits -> up to 3, rounding digits -> up to 4
		/// </summary>
		/// <param name="degrees">The value of degrees to find a sine from</param>
		/// <returns>Float of interpolated sine value/returns>
		constexpr float sinDeg_tableInterp(const float degrees) noexcept {
			const llint angleF = ace::math::truncToInt(degrees); //truncated angle
			return ace::math::lerp<float>(sinTable[ace::math::_getTrigTableIndex(angleF)], sinTable[ace::math::_getTrigTableIndex(angleF+1)], degrees - angleF);
		}

		/// <summary>
		/// Calculates the interpolated sine value from the cosine lookup table
		/// @note Correct to the cosine value up to a few decimal point digits: truncating digits -> up to 3, rounding digits -> up to 4
		/// </summary>
		/// <param name="degrees">The value of degrees to find a cosine from</param>
		/// <returns>Float of interpolated cosine value/returns>
		constexpr float cosDeg_tableInterp(const float degrees) noexcept {
			const llint angleF = ace::math::truncToInt(degrees); //truncated angle
			return ace::math::lerp<float>(cosTable[ace::math::_getTrigTableIndex(angleF)], cosTable[ace::math::_getTrigTableIndex(angleF+1)], degrees - angleF);
		}

		/// <summary>
		/// Calculates the interpolated sine value from the tangent lookup table
		/// @note Correct to the tangent value up to a few decimal point digits: truncating digits -> up to 3, rounding digits -> up to 4
		/// </summary>
		/// <param name="degrees">The value of degrees to find a tangent from</param>
		/// <returns>Float of interpolated tangent value/returns>
		constexpr float tanDeg_tableInterp(const float degrees) noexcept {
			const llint angleF = ace::math::truncToInt(degrees); //truncated angle
			return ace::math::lerp<float>(tanTable[ace::math::_getTrigTableIndex(angleF)], tanTable[ace::math::_getTrigTableIndex(angleF+1)], degrees - angleF);
		}

		/// <summary>
		/// Calculates the interpolated sine value from the cotangent lookup table
		/// @note Correct to the cotangent value up to a few decimal point digits: truncating digits -> up to 3, rounding digits -> up to 4
		/// </summary>
		/// <param name="degrees">The value of degrees to find a cotangent from</param>
		/// <returns>Float of interpolated cotangent value/returns>
		constexpr float cotDeg_tableInterp(const float degrees) noexcept {
			const llint angleF = ace::math::truncToInt(degrees); //truncated angle
			return ace::math::lerp<float>(cotTable[ace::math::_getTrigTableIndex(angleF)], cotTable[ace::math::_getTrigTableIndex(angleF+1)], degrees - angleF);
		}

		/// <summary>
		/// Approximates the interpolated sine value from the cosecant lookup table
		/// @note Correct to the cosecant value up to a few decimal point digits: truncating digits -> up to 3, rounding digits -> up to 4
		/// </summary>
		/// <param name="degrees">The value of degrees to find a cosecant from</param>
		/// <returns>Float of interpolated cosecant value/returns>
		constexpr float cscDeg_tableInterp(const float degrees) noexcept {
			const llint angleF = ace::math::truncToInt(degrees); //truncated angle
			return ace::math::lerp<float>(cscTable[ace::math::_getTrigTableIndex(angleF)], cscTable[ace::math::_getTrigTableIndex(angleF+1)], degrees - angleF);
		}

		/// <summary>
		/// Calculates the interpolated sine value from the secant lookup table
		/// @note Correct to the secant value up to a few decimal point digits: truncating digits -> up to 3, rounding digits -> up to 4
		/// </summary>
		/// <param name="degrees">The value of degrees to find a secant from</param>
		/// <returns>Float of interpolated secant value/returns>
		constexpr float secDeg_tableInterp(const float degrees) noexcept {
			const llint angleF = ace::math::truncToInt(degrees); //truncated angle
			return ace::math::lerp<float>(secTable[ace::math::_getTrigTableIndex(angleF)], secTable[ace::math::_getTrigTableIndex(angleF+1)], degrees - angleF);
		}
	}
}

#endif // ENGINE_ENABLE_MATH_LOOKUP

#endif // !ENGINE_TRIG_LOOKUP_HPP
