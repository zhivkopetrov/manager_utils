#ifndef MANAGER_UTILS_IMAGE_H_
#define MANAGER_UTILS_IMAGE_H_

/*
 * Image.h
 * @brief
 *  A class used for drawing. It is used as wrapper of the drawing library
 */

// System headers

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/Widget.h"

// Forward declarations
class Sprite;

class Image : public Widget {
 public:
  Image();
  ~Image() noexcept;

  Image(Image&& movedOther);
  Image& operator=(Image&& movedOther);

  /** @brief used to create resource. This function must be called
   *         in order to operate will the resource. The Image itself only
   *         handles draw specific data, not the actual Surface/Texture
   *   This function does not return error code for performance reasons
   *
   *  @param const uint64_t - unique resource ID
   * */
  void create(const uint64_t rsrcId);

  /** @brief used to destroy Image texture. In order to use the Image
   *         texture again - it needs to be re-created.
   *                                      (Invoking of .create() method)
   * */
  void destroy();

  /** @brief used to set(swap an existing texture for an
   *         already created resource).
   *
   *         NOTE: This function may only be called after the resource was
   *               initially created.
   *
   *         WARNINING: The function destroys previous resource sprites,
   *                    and loads the new resources sprites.
   *                    Although the old resource is destroyed it's
   *                    Widget params are still present,
   *                    They are used to update the sprite for
   *                    the new loaded resource.
   *
   *  @param const uint64_t - unique resource ID
   * */
  void setTexture(const uint64_t rsrcId);

  /**
   * @brief a getter, so the recource ID is public information
   */
  uint64_t getRsrcId() const { return _drawParams.rsrcId; }

  /** @brief used to set Image Rect to point to a specific frame Rect.
   *         Even if Image does not have sprites it is still valid to
   *         call setFrame(0)
   *
   *  @param const int32_t - input frame index
   * */
  void setFrame(const int32_t frameIndex);

  /** @brief used to set next valid frame index.
   *         NOTE: if maxFrames are reached frame 0 is set.
   * */
  void setNextFrame();

  /** @brief used to set previous valid frame index.
   *         NOTE: if -1 index is reached frame _maxFrames - 1 is set.
   * */
  void setPrevFrame();

  /** @brief used to manually add an additional frame to the Image
   *          Constrains: the frame rectangle must be from the
   *                       same source file image as the original frames
   *  @const Rectangle & - new frame Rectangle dimensions
   * */
  void addFrame(const Rectangle& rectFrame);

  /** @brief used to manually set frame rectangles for the currently
   *         created widget rsrcId
   *
   *  @param const Rectangle * frameRects - new frame rectangles
   *  @param const uint32_t               - rectangles count
   *
   *         WARNING: invocation of this method overrides previous frame
   *                  rectangles. Use with caution.
   * */
  void setManualFrames(const Rectangle* frameRects,
                       const uint32_t rectanglesCount);

  /** @brief used to obtain currently set frame index
   *
   *  @param int32_t - frame index
   * */
  int32_t getFrame() const;

  /** @brief used to obtain maximum frame count
   *
   *  @param int32_t - frame index
   * */
  int32_t getFrameCount() const;

 protected:
  // Holds sprites for the Image
  Sprite* _sprites;

  /* used in order to check if resource was destroyed ->
   *                                              not to destroy it twice
   */
  bool _isDestroyed;
};

#endif /* MANAGER_UTILS_IMAGE_H_ */
