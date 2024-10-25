/*=========================================================================*/
/*                                                                         */
/* Copyright (c) 2021  Fingerprint Cards AB                                */
/*                                                                         */
/*=========================================================================*/

#pragma once

#ifndef __iris_engine_v3_h__
#define __iris_engine_v3_h__

#include <stdlib.h>
#include <stdint.h>

#ifdef __CDT_PARSER__
#undef __cplusplus
#endif

/* library version */
#define IRE3_VER_LIBRARY   (1)
/* feature template version */
#define IRE3_VER_TEMPLATE  (2)

/*
 * major                major version
 * minor                minor version
 * build                build number (or IRE3_VALUE_UNKNOWN)
 * date_code            date of the release YYYYMMDD e.g. 20150131
 */
typedef struct tag_ire3_version {
  int major;
  int minor;
  int build;
  int date_code;
} ire3_version;

#define IRE3_VALUE_UNKNOWN                   (-1)
#define IRE3_VALUE_GOOD                      (0)

#define IRE3_STATUS_OK                       (0x00000000)
#define IRE3_STATUS_MATCH                    (0x00000000)
#define IRE3_STATUS_NOT_MATCH                (0x00000001)
#define IRE3_STATUS_FAIL                     (0x00000001)

#define IRE3_ERR_DECODING_FEATURES           (0x80000005)
#define IRE3_ERR_ENCODING_FEATURES           (0x80000004)
#define IRE3_ERR_INVALID_PARAM               (0x80000003)
#define IRE3_ERR_OUTOFMEMORY                 (0x80000002)
#define IRE3_ERR_NOT_IMPLEMENTED             (0x80000001)
#define IRE3_ERR_OFFSET_ALIGNMENT_VIOLATION  (0x80070147) // __HRESULT_FROM_WIN32(ERROR_OFFSET_ALIGNMENT_VIOLATION)
#define IRE3_ERR_INVALID_FEATURES            (0x8F100000)

#define IRE3_ERR_INVALID_LICENSE             (0x80000100)

#define IRE3_FAILED(rc)                      (((int)(rc))  < 0)
#define IRE3_SUCCEEDED(rc)                   (((int)(rc)) >= 0)

/*
 * iris_center_x
 * iris_center_y        iris center x and y co-ordinates
 *                        or IRE3_VALUE_UNKNOWN if iris was not found
 * iris_diameter        iris diameter
 *                        or IRE3_VALUE_UNKNOWN if iris was not found
 * pupil_center_x
 * pupil_center_y       pupil center x and y co-ordinates
 *                        or IRE3_VALUE_UNKNOWN if pupil was not found
 * pupil_diameter       pupil diameter
 *                        or IRE3_VALUE_UNKNOWN if pupil was not found
 */
typedef struct tag_ire3_eye_info {
  int iris_center_x;
  int iris_center_y;
  int iris_diameter;
  int pupil_center_x;
  int pupil_center_y;
  int pupil_diameter;
  int percent_occlusion;
  int sharpness;
} ire3_eye_info;

/*
* out_image_width       width of the output kind 3/7 image
* out_image_height      height of the output kind 3/7 image
* kind3_pixels          pointer to store kind3 image
* kind7_pixels          pointer to store kind7 image
* If <>_pixels is NULL no images are saved.
*/
typedef struct tag_ire3_out_images {
  int out_image_width;
  int out_image_height;
  uint8_t *kind3_pixels;
  uint8_t *kind7_pixels;
} ire3_out_images;

/*===========================================================================*/
/* size                 size of this structure                               */
/* nominal_resolution   resolution in pixels per cm at the nominal           */
/*                        working distance (where the camera is focused)     */
/*                        range: [120, 340]                                  */
/* angular_tolerance:   ~ +/- range of angles for comparison, 1...16,        */
/*                        where 16 => +/-45 degrees, average comparison time */
/*                        is ~ to angular_tolerance                          */
/*===========================================================================*/
typedef struct _tag_ire3_settings {
  int size;
  int nominal_resolution;
  int angular_tolerance;
  void *ptr;
} ire3_settings;

#if defined(__cplusplus)
extern "C" {
#endif

  /*
   * @fn ire3_get_version: returns the version number
   * @input: which_version: one of the version types defined with prefix IRE3_VER_
   * @input: version: pointer to structure of type ire3_version to store version information
   * @input: version_size: size of the version
   * @output: version information
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_get_version(int which_version, ire3_version *version, size_t version_size);

  /*
   * @fn ire3_get_extraction_working_set_size: returns the working set size in uint8s
   * @input: size: the pointer to size_t to store working set size
   * @input: settings: angular_tolerance, nominal_resolution
   * @output: size value: working set size
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_get_extraction_working_set_size(size_t *size, const ire3_settings *settings);

  /*
   * @fn ire3_get_max_features_size: the max size (in uint8s) of the features computed from a single image
   * @input: size: the pointer to integer to store feature size
   * @output: size value: features size
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_get_max_features_size(size_t *size);

  /*
   * @fn ire3_extract_features: extracts the feature from an image
   * @input: pixels: grayscale 8bpp image pixel array
   * @input: width: width of the image
   * @input: height: height of the image
   * @input: features: memory buffer store the features
   * @input: features_buffer_size: size of memory buffer to store the features
   * @input: features_size: pointer to store actual size of features extracted
   * @input: eye_info: pointer to store eye location
   * @input: working_set: pointer to working set memory provided by caller
   * @input: working_set_size: size of the working set
   * @input: settings: angular_tolerance, nominal_resolution
   * @output: features: features extracted from the image
   * @output: features_size: actual size of features extracted
   * @output: eye_info: eye location
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_extract_features(const unsigned char *pixels, int width, int height,
                               unsigned char *features, size_t features_buffer_size, size_t * features_size,
                               ire3_eye_info * eye_info, size_t eye_info_size,
                               void * working_set, size_t working_set_size, const ire3_settings *settings);
  /*
   * @fn ire3_extract_features: extracts the feature from an image, produces additional kind3/7 images
   * @input: pixels: grayscale 8bpp image pixel array
   * @input: width: width of the image
   * @input: height: height of the image
   * @input: features: memory buffer store the features
   * @input: features_buffer_size: size of memory buffer to store the features
   * @input: features_size: pointer to store actual size of features extracted
   * @input: eye_info: pointer to store eye location
   * @input: working_set: pointer to working set memory provided by caller
   * @input: working_set_size: size of the working set
   * @input: settings: angular_tolerance, nominal_resolution
   * @output: features: features extracted from the image
   * @output: features_size: actual size of features extracted
   * @output: eye_info: eye location
   * @output: ire3_out_images: kind3 and kind7 images in NOT NULL
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_extract_features_with_images(const unsigned char *pixels, int width, int height,
                               unsigned char *features, size_t features_buffer_size, size_t * features_size,
                               ire3_eye_info * eye_info, size_t eye_info_size,
                               void * working_set, size_t working_set_size, const ire3_settings *settings, 
                               ire3_out_images *images);
  /*
   * @fn ire3_get_deserialized_features_size: returns deserialized features size in bytes
   * @output: size: amount of memory in bytes to allocate for the deserialized features object
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_get_deserialized_features_size(size_t * size);

  /*
   * @fn ire3_deserialize_features: deserializes features
   * @input: features: features to deserialize
   * @input: features_size: size in bytes of features to deserialize
   * @output: ftr_obj: memory buffer for the deserialized features
   * @input: ftr_obj_size: size in bytes of the memory buffer for the deserialized features
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   */
  int ire3_deserialize_features(const unsigned char * features, size_t features_size, void * ftr_obj, size_t ftr_obj_size);

  /*
   * @fn ire3_compare: slow one-to-one comparison, computes the similariry score
   * @input: ftr_obj_1: deserialized features to compare
   * @input: ftr_obj_2: deserialized features on file (template)
   * @input: score: pointer integer storing the score value
   * @output: score:[0,10000]. score value is normalized as -100 Log10(fmr)
   * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
   * Notes: this function does not modify features object and it is safe to call multiple ire3_compare concurrently
   */
  int ire3_compare(const void *ftr_obj_1, const void *ftr_obj_2, const ire3_settings *settings, int *score);

  /*
  * @fn ire3_get_features_version: returns the version number
  * @input: ftr_obj: deserialized features to query version
  * @input: which_version: one of the version types defined with prefix IRE3_VER_
  * @input: version: pointer to structure of type ire3_version to store version information
  * @input: version_size: size of the version
  * @output: version information
  * @return: return code IRE3_STATUS_OK or one of IRE3_ERR_
  */
  int ire3_get_features_version(void * ftr_obj, int which_version, ire3_version *version, size_t version_size);

  /*
   * @fn ire3_init: initialize ire3_settings struct
   * @input: nominal_resolution: nomial resolution
   * @input: angular_tolerance: angular tolerance
   * @input: lic: pointer to license
   * @output: settings: ire3_settings struct
   */
  int ire3_init(int nominal_resolution, int angular_tolerance, char *lic, ire3_settings *settings);

  /*
   * @fn ire3_deinit: free ire3_settings_struct
   * @input settings: ire3_settings_struct
   */
   int ire3_deinit(ire3_settings *settings);

#if defined(__cplusplus)
}
#endif

#endif // __iris_engine_v3_h__
