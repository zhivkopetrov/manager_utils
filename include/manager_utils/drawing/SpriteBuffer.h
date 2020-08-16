#ifndef MANAGER_UTILS_SPRITEBUFFER_H_
#define MANAGER_UTILS_SPRITEBUFFER_H_

/*
 * SpriteBuffer.h
 *
 *  Brief: The SpriteBuffer is used for run time generation
 *         of SDL_Surface/SDL_Texture with 1 or (usually) more widgets.
 *
 *         SpriteBuffer is used for optimisation purposes when the developer
 *         know that several graphical widgets that share a common logical
 *         rectangle boundaries will be constantly drawn, but not updated.
 *
 *         For not to every time make those N draw calls for your N widgets
 *         on every draw frame, instead the developer creates a SpriteBuffer
 *         by "drawing all those N widgets with ::addWidget() method" once.
 *
 *         From that point on your make only a single draw call on the
 *         SpriteBuffer.
 *
 *         After some time when something actually changes for one (or several)
 *         of those N widgets you simply: update your widgets, reset your old
 *         SpriteBuffer, update it and then continue to draw it again.
 *
 *
 *
 *     Example for usage:
 *
 *     class Container
 *     {
 *         public:
 *             int32_t init();
 *
 *             void updateSomeLogic();
 *
 *             void draw();
 *
 *         private:
 *         Image img1;
 *         Image img2;
 *         Text  text;
 *
 *         SpriteBuffer spriteBuffer;
 *     }
 *
 *     int32_t init()
 *     {
 *         img1.create();
 *         img2.create();
 *         text.create( "Eclipse > CodeBlocks", some other params ... );
 *
 *         SpriteBuffer spriteBuffer;
 *         if(SUCCESS != spriteBuffer.create())
 *         {
 *              LOGERR("spriteBuffer.create() failed")
 *              return FAILURE;
 *         }
 *
 *         spriteBuffer.addWidget(img);
 *         spriteBuffer.addWidget(img2);
 *         spriteBuffer.addWidget(text);
 *
 *         //lock and unlock are only needed for the ::update method()
 *         spriteBuffer.unlock();
 *         spriteBuffer.update();
 *         spriteBuffer.lock();
 *
 *         return SUCCESS;
 *     }
 *
 *     void draw()
 *     {
 *         g.drawWidget(spriteBuffer);
 *     }
 *
 *     void updateSomeLogic()
 *     {
 *         //update your business logic
 *         img1.moveDown(5);
 *
 *         img2.moveLeft(10);
 *
 *         text.setText("Bazinga");
 *
 *         //Option 1:
 *         //cleaner code but slower execution -> wrap ::reset() and ::update()
 *         //methods between ::unlock() and lock() functions
 *         {
 *             //reset your old SpriteBuffer
 *             spriteBuffer.unlock();
 *             spriteBuffer.reset();
 *             spriteBuffer.lock();
 *
 *             //add your new Widgets
 *             spriteBuffer.addWidget(img);
 *             spriteBuffer.addWidget(img2);
 *             spriteBuffer.addWidget(text);
 *
 *             spriteBuffer.unlock();
 *             spriteBuffer.update();
 *             spriteBuffer.lock();
 *         }
 *
 *
 *         //Option 2:
 *         //more error prone code but faster execution -> call ::unlock() at
 *         //the beginning, reset old SpriteBuffer, add new Widgets, update
 *         //(generate new SpriteBuffer) and then call ::lock() at the end
 *         {
 *             //reset your old SpriteBuffer
 *             spriteBuffer.unlock();
 *             spriteBuffer.reset();
 *
 *             //add your new Widgets
 *             spriteBuffer.addWidget(img);
 *             spriteBuffer.addWidget(img2);
 *             spriteBuffer.addWidget(text);
 *
 *             spriteBuffer.update();
 *             spriteBuffer.lock();
 *         }
 *     }
 */

// C system headers

// C++ system headers
#include <cstdint>
#include <vector>

// Other libraries headers

// Own components headers
#include "manager_utils/drawing/Widget.h"
#include "utils/drawing/Color.h"

// Forward declarations

class SpriteBuffer : public Widget {
 public:
  SpriteBuffer();
  ~SpriteBuffer();

  // move constructor needed for STL containers empalce_back/push_back
  SpriteBuffer(SpriteBuffer&& movedOther);

  // move assignment operator implementation
  SpriteBuffer& operator=(SpriteBuffer&& movedOther);

  // forbid the copy constructor and copy assignment operator
  SpriteBuffer(const SpriteBuffer& other) = delete;
  SpriteBuffer& operator=(const SpriteBuffer& other) = delete;

  /** @brief used to create an empty SpriteBuffer with the given params.
   *
   *  @param const int32_t - X coordinate for the SpriteBuffer
   *  @param const int32_t - Y coordinate for the SpriteBuffer
   *  @param const int32_t - width for the SpriteBuffer (final texture)
   *  @param const int32_t - height for the SpriteBuffer (final texture)
   *                                          SpriteBuffer is attached to
   *  @param const double  - rotation angle (if such is used)
   *  @param const Point   - rotation center (if such is used)
   * */
  void create(const int32_t coordinateX, const int32_t coordinateY,
              const int32_t spriteBufferWidth, const int32_t spriteBufferHeight,
              const double rotationAngle = ZERO_ANGLE,
              const Point& rotationCenter = Point::UNDEFINED);

  /* override of 6 param create method */
  void create(const Rectangle& dimensions,
              const double rotationAngle = ZERO_ANGLE,
              const Point& rotationCenter = Point::UNDEFINED);

  /** @brief used to destroy the SpriteBuffer
   * */
  void destroy();

  /** @brief used for unlocking the SpriteBuffer before invoking
   *                                                    ::update() method
   * */
  void unlock();

  /** @brief used for locking the SpriteBuffer after
   *                                  ::update() method has been executed
   * */
  void lock();

  /** @brief used to clear (wipe out) the content of the current
   *         SpriteBuffer and clear the content
   *                                      of _storedItems std::vector
   *
   *         NOTE: each call to ::reset() method must be wrapped between
   *         ::unlock() and ::lock() methods
   * */
  void reset();

  /** @brief used to upload new Graphical Widget in the SpriteBuffer.
   *         The Widget can represent Image/Text/SpriteBuffer
   *
   *  @param const Widget & - the Widget that is about to be uploaded
   *                                                  to the SpriteBuffer
   *
   *         WARNING:  Be careful if you invoke addWidget() for another
   *                   SpriteBuffer as a parameter, because SpriteBuffers,
   *                   deallocated memory for their surfaces/textures
   *                   when destroyed.
   *                   If you happen to call addWidget() for another
   *                   SpriteBuffer be sure to not call the ::update()
   *                   method if the destroyed SpriteBuffer DrawParams
   *                   are still part of the _storedItem std::vector.
   *                   It is safe however to call ::reset() and then
   *                   continue with your business logic.
   *
   *         WARNING2: Widget param needs to be created before addWidget()
   *                   method is invoked. If it was not created - the
   *                   upload to the SpriteBuffer will fail
   *                                                  (will be skipped)
   * */
  void addWidget(const Widget& widget);

  /** @brief used to override the existing SpriteBuffer final texture
   *         with Surfaces/Textures from the _storedItems std::vector
   *
   *         NOTE: each call to ::update() method must be wrapped between
   *         ::unlock() and ::lock() methods
   *  */
  void update();

  /** @brief used to override the existing SpriteBuffer final texture
   *         with Surfaces/Textures from the _storedItems std::vector
   *
   *  @param const int32_t - begin stored item index
   *  @param const int32_t - end stored item index
   *
   *         NOTE: each call to ::updateRanged() method must be wrapped
   *         between ::unlock() and ::lock() methods
   *  */
  void updateRanged(const int32_t fromIndex, const int32_t toIndex);

  /** @brief used to set user defined clear target (clear mechanism for
   *         the spriteBuffer). If such is not set - the default renderer
   *         clear mechanism is used.
   *
   *  @param const Widget & - clear target widget
   *
   *         WARNING: if the developer plans to move the spriteBuffer
   *                  (and/or his items) position after it has been
   *                  initially created - the custom clear target position
   *                  should be moved as well.
   * */
  void addCustomClearTarget(const Widget& widget);

  /** @brief used to change the clear colour for the Hardware accelerated
   *         renderer when ::reset() method is called
   *                                  (and no custom clear target is set)
   *
   *  @param const Color & - renderer clear colour
   * */
  void setSpriteBufferResetColor(const Color& clearColor);

  /** @brief used to move all stored SpriteBuffer items (widgets)
   *         with relative offset
   *
   *  @param const int32_t - relative X offset
   * */
  void moveItemsRight(const int32_t x) { _itemsOffsetX += x; }

  /** @brief used to move all stored SpriteBuffer items (widgets)
   *         with relative offset
   *
   *  @param const int32_t - relative X offset
   * */
  void moveItemsLeft(const int32_t x) { _itemsOffsetX -= x; }

  /** @brief used to move all stored SpriteBuffer items (widgets)
   *         with relative offset
   *
   *  @param const int32_t - relative Y offset
   * */
  void moveItemsDown(const int32_t y) { _itemsOffsetY += y; }

  /** @brief used to move all stored SpriteBuffer items (widgets)
   *         with relative offset
   *
   *  @param const int32_t - relative Y offset
   * */
  void moveItemsUp(const int32_t y) { _itemsOffsetY -= y; }

  uint64_t getStoredItemsCount() const { return _storedItems.size(); }

 private:
  /** @brief used to transform relative sprite buffer coordinates to
   *         relative ones for the monitor, on which the SpriteBuffer
   *         is attached to
   *
   *  @param const uint32_t - stored items size for the sprite buffer
   * */
  void transformToMonitorRelativeCoordinates(const uint32_t storedItemsSize);

  /** @brief used to transform relative sprite buffer coordinates to
   *         relative ones for the monitor, on which the SpriteBuffer
   *         is attached to
   *
   *  @param const int32_t - begin stored item index
   *  @param const int32_t - end stored item index
   *
   *         NOTE: internal boundary checking is not made, because it is
   *               supposed to be caught before that in the
   *               call tree hierarchy.
   * */
  void transformToMonitorRelativeCoordinatesRanged(const int32_t fromIndex,
                                                   const int32_t toIndex);

  /** @brief used to reset internal variables
   * */
  void resetInternals();

  /** Holds all Widget DrawParams used by ::addWidget() method.
   * When ::update() is called the final Surface/Texture is created from
   * all stored items
   * */
  std::vector<DrawParams> _storedItems;

  /* Holds custom clear target data (if such is provided) */
  DrawParams _customClearTarget;

  /* Holds clear color for when ::reset() method is invoked and no
   * custom clear target is set
   * */
  Color _clearColor;

  /* Stores relative offset for stored items(widgets) in the SpriteBuffer
   * */
  int32_t _itemsOffsetX;
  int32_t _itemsOffsetY;

  /* Holds locked/unlocked status of the sprite buffer */
  bool _isLocked;

  /* Used to determine whether to use the default renderer clear
   * mechanism or there is user provided one
   * */
  bool _isCustomClearTargetSet;

  /* Used to determine whether the sprite buffer has
   * already been destroyed
   * */
  bool _isDestroyed;
};

#endif /* MANAGER_UTILS_SPRITEBUFFER_H_ */
