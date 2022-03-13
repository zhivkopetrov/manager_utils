#ifndef MANAGER_UTILS_DYNAMICIMAGE_H_
#define MANAGER_UTILS_DYNAMICIMAGE_H_

// System headers

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/Image.h"

// Forward declarations

class DynamicImage : public Image {
 public:
  ~DynamicImage();

  DynamicImage(DynamicImage&& movedOther);
  DynamicImage& operator=(DynamicImage&& movedOther);

  /** @brief used to create resource. This function must be called
   *         in order to operate will the resource. The Image itself only
   *         handles draw specific data, not the actual Surface/Texture
   *   This function does not return error code for performance reasons
   *
   *   WARNING: as opposed to the Image::create() method, the DynamicImage
   *            ::create() method actually calls dynamic resource load.
   *
   *   NOTE: The underlying implementation uses reference counting.
   *         What this means is when you invoke ::create() several times
   *         on the same resourceId only 1 unique instance of the resource
   *         will be loaded into memory.
   *
   *   NOTE2: you if have the following case:
   *          DynamicImage::loadResourceOnDemandSingle(someRsrcId1);
   *          DynamicImage::loadResourceOnDemandSingle(someRsrcId2);
   *          DynamicImage::loadResourceOnDemandSingle(someRsrcId3);
   *          DynamicImage::loadResourceOnDemandSingle(someRsrcId4);
   *
   *          This will massacre the performance and the engine will
   *          experience a heavy FPS drop. To use the multithreading
   *          approach and extract the maximum power out of the platform
   *          do the following:
   *
   *          const std::vector RSRC_IDS {someRsrcId1, someRsrcId2,
   *                                            someRsrcId3, someRsrcId4};
   *
   *          gRsrcMgr->loadResourceOnDemandMultiple(RSRC_IDS);
   *
   *          This will dispatch the CPU specific work to other worker threads
   *          that are already waiting on a condition variable.
   *
   *          The main thread will consume their work and start to
   *          upload to the GPU simultaneously as the worker threads
   *          are doing their CPU work.
   *
   *   NOTE3: If loadResourceOnDemandMultiple() is used it is up to the
   *          developer to call unloadResourceOnDemandMultiple() on the
   *          same resourceIds after the work is finished;
   *
   *  @param const uint64_t - unique resource ID
   * */
  void create(const uint64_t rsrcId);

  /** @brief used to destroy DynamicImage texture.  In order to use the
   *         DynamicImage texture again - it needs to be re-created.
   *                                      (Invoking of .create() method)
   *
   *  WARNING: as opposed to the Image::destroy() method, the DynamicImage
   *            ::destroy() method actually calls dynamic resource unload.
   *
   *   NOTE: The underlying implementation uses reference counting.
   *         What this means is when you invoke ::destroy() several times
   *         on the same resourceId - the unique instance of the resource
   *         will be unloaded from memory only once.
   *
   *   NOTE2: if your implementation relies on invoking ::destroy() on
   *          a DynamicImage - you are probably using the wrong
   *          implementation. Proceed with caution!
   * */
  void destroy();
};

#endif /* MANAGER_UTILS_DYNAMICIMAGE_H_ */
