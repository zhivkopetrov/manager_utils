#ifndef MANAGER_UTILS_TEXT_H_
#define MANAGER_UTILS_TEXT_H_

// System headers
#include <cstdint>

// Other libraries headers
#include "utils/drawing/Color.h"

// Own components headers
#include "manager_utils/drawing/Widget.h"

// Forward Declarations

class Text : public Widget {
 public:
  Text();
  ~Text() noexcept;

  Text(Text&& movedOther);
  Text& operator=(Text&& movedOther);

  /** @brief used to create resource. This function must be called
   *         in order to operate will be resource. The Text itself only
   *         handles draw specific data, not the actual Surface/Texture
   *   This function does not return error code for performance reasons
   *
   *  @param const fontId - unique resource ID
   *  @param const char *   - text content we want to create as Text image
   *  @param const Color &  - color we want to create the Text with
   *  @param const Point    - the position we want the Text to be created
   * */
  void create(const uint64_t fontId,
              const char* text,
              const Color& color,
              const Point& pos = Points::ZERO);

  /** @brief used to destroy Text texture. In order to use the text
   *         texture again - it needs to be re-created.
   *                                      (Invoking of .create() method)
   * */
  void destroy();

  /** @brief used to set new content of Text texture.
   *         If new text content differs from the current one
   *         internally the old Text surface/texture is destroyed and
   *         new one is created with input text content.
   *
   *  @param const char * - new text content to be displayed
   * */
  void setText(const char* text);

  /** @brief used to set color for Text texture.
   *         Internally the old Text surface/texture is destroyed and
   *         new one is created with input new text color.
   *
   *  @param const Color & - new color of the text
   * */
  void setColor(const Color& color);

  /** @brief combination of ::setText() and ::setColor()
   *
   *         NOTE: if you need to change both of them at the same time
   *               it is better to use this method instead of the listed
   *               two, because each of them destroys the text and
   *               creates new one.
   *
   *  @param const char * - new text content to be displayed
   *  @param const Color & - new color of the text
   * */
  void setTextAndColor(const char* text, const Color& color);

  /** @brief used to get C-style char array to the _textContent
   *         currently being hold by the Text instance
   *
   *  @returns const char*  - text content of the Text instance
   * */
  const char* getText() const { return _textContent; }

 private:
  void resetInternals();

  /** @brief used to make a deep copy of the input text
   * */
  void copyTextContent(const char* text);

  /** Since Text header will be included a lot -> try not to
   *  include std::string. This will heavily influence the compile time
   *  for every file that includes the Text header.
   *  */
  const char* _textContent;

  // Holds the font id correcposnding to the current text
  uint64_t _fontId;

  // The color of the text
  Color _color;

  /* used in order to check if resource was destroyed ->
   *                                              not to destroy it twice
   */
  bool _isDestroyed;
};

#endif /* MANAGER_UTILS_TEXT_H_ */
