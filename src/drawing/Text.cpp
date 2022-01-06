// Corresponding header
#include "manager_utils/drawing/Text.h"

// C system headers

// C++ system headers
#include <cstring>
#include <utility>

// Other libraries headers

// Own components headers
#include "manager_utils/managers/RsrcMgr.h"
#include "utils/LimitValues.hpp"
#include "utils/Log.h"

// default constructor
Text::Text()
    : _textContent(nullptr),
      _fontId(INIT_UINT64_VALUE),
      _color(Colors::BLACK),
      _isDestroyed(false){
  _drawParams.widgetType = WidgetType::TEXT;
}

// move constructor
Text::Text(Text&& movedOther)
    : Widget(std::move(movedOther)),
      _color(Colors::BLACK) {
// explicitly call Color default copy/move constructor
// because the default constructor is forbidden
  _drawParams.widgetType = WidgetType::TEXT;

  // take ownership of resources
  _textContent = movedOther._textContent;
  _fontId = movedOther._fontId;
  _color = movedOther._color;
  _isDestroyed = movedOther._isDestroyed;

  // ownership of resource should be taken from moved instance
  movedOther._textContent = nullptr;
  movedOther._fontId = INIT_UINT64_VALUE;
  movedOther._color = Colors::BLACK;
  movedOther._isDestroyed = false;
}

// move assignment operator
Text& Text::operator=(Text&& movedOther) {
  // check for self-assignment
  if (this != &movedOther) {
    _drawParams.widgetType = WidgetType::TEXT;

    // take ownership of resources
    _textContent = movedOther._textContent;
    _fontId = movedOther._fontId;
    _color = movedOther._color;
    _isDestroyed = movedOther._isDestroyed;

    // explicitly invoke Widget's move assignment operator
    Widget::operator=(std::move(movedOther));

    // ownership of resource should be taken from moved instance
    movedOther._textContent = nullptr;
    movedOther._fontId = INIT_UINT64_VALUE;
    movedOther._color = Colors::BLACK;
    movedOther._isDestroyed = false;
  }

  return *this;
}

Text::~Text() {
  // attempt to destroy text only if it's was first created and not destroyed
  if (true == _isCreated && false == _isDestroyed) {
    destroy();
  }
}

void Text::create(const uint64_t fontId,
                  const char* text,
                  const Color& color,
                  const Point& pos) {
  if (_isCreated) {
    LOGERR(
        "Warning, trying to create a text that was already created with "
        "fontId: %#16lX", _fontId);
    return;
  }

  _isCreated = true;
  _isDestroyed = false;
  _fontId = fontId;
  _color = color;
  _drawParams.pos.x = pos.x;
  _drawParams.pos.y = pos.y;

  copyTextContent(text);

  gRsrcMgr->loadText(_fontId, _textContent, _color,
                         _drawParams.textId, _imageWidth, _imageHeight);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled).
   * Since texts are stand-alone texture (they don't have sprites) ->
   * we simulate they do as masking the whole surface/texture as a
   * sprite list with a single sprite in it with
   * frameRect.x = 0 and frameRect.y = 0.
   * */
  setFrameRect(Rectangle(0,               // frameRect.x
                         0,               // frameRect.y
                         _imageWidth,     // frameRect.w
                         _imageHeight));  // frameRect.h
}

void Text::destroy() {
  if (_isDestroyed) {
    LOGERR(
        "Warning, trying to destroy already destroyed text with fontId: "
        "%#16lX", _fontId);
    return;
  }

  if (!_isCreated) {
    LOGERR(
        "Warning, trying to destroy a not-created text with fontId: "
        "%#16lX", _fontId);
    return;
  }

  _isDestroyed = true;

  // sanity check, because manager could already been destroyed
  if (nullptr != gRsrcMgr) {
    // unload text from graphical text vector
    gRsrcMgr->unloadText(_drawParams.textId);
  }

  if (nullptr != _textContent) {
    delete[] _textContent;
    _textContent = nullptr;
  }

  Widget::reset();
}

void Text::setText(const char* text) {
  if (!_isCreated) {
    LOGERR("Error, text with fontId: %#16lX not created!", _fontId);
    return;
  }

  /** No sanity check if needed for _textContent since it's guarded by
   *  _isCreated flag. Since .setText() will be called a lot -> better to
   *  avoid unnecessary overhead.
   * */

  // compare strings only if their size is equal
  if (strlen(_textContent) == strlen(text)) {
    if (0 == strcmp(_textContent, text)) {
      // strings are equal -> no need to re-create the text texture
      return;
    }
  }

  copyTextContent(text);

  // setting new text required freeing old resources, allocating new ones
  // and creating new surface/textures
  gRsrcMgr->reloadText(_fontId, _textContent, _color,
                       _drawParams.textId, _imageWidth, _imageHeight);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled).
   * Since texts are stand-alone texture (they don't have sprites) ->
   * we simulate they do as masking the whole surface/texture as a
   * sprite list with a single sprite in it with
   * frameRect.x = 0 and frameRect.y = 0.
   * */
  setFrameRect(Rectangle(0,               // frameRect.x
                         0,               // frameRect.y
                         _imageWidth,     // frameRect.w
                         _imageHeight));  // frameRect.h
}

void Text::setColor(const Color& color) {
  if (!_isCreated) {
    LOGERR("Error, text with fontId: %#16lX not created!", _fontId);
    return;
  }

  _color = color;
  // setting new text required freeing old resources, allocating new ones
  // and creating new surface/textures
  gRsrcMgr->reloadText(_fontId, _textContent, _color,
                       _drawParams.textId, _imageWidth, _imageHeight);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled).
   * Since texts are stand-alone texture (they don't have sprites) ->
   * we simulate they do as masking the whole surface/texture as a
   * sprite list with a single sprite in it with
   * frameRect.x = 0 and frameRect.y = 0.
   * */
  setFrameRect(Rectangle(0,               // frameRect.x
                         0,               // frameRect.y
                         _imageWidth,     // frameRect.w
                         _imageHeight));  // frameRect.
}

void Text::setTextAndColor(const char* text, const Color& color) {
  if (!_isCreated) {
    LOGERR("Error, text with fontId: %#16lX not created!", _fontId);
    return;
  }

  copyTextContent(text);

  _color = color;
  // setting new text required freeing old resources, allocating new ones
  // and creating new surface/textures
  gRsrcMgr->reloadText(_fontId, _textContent, _color,
                       _drawParams.textId, _imageWidth, _imageHeight);

  /** Explicitly call setFrameRect() method in order to invoke any
   * crop modification (if such is enabled).
   * Since texts are stand-alone texture (they don't have sprites) ->
   * we simulate they do as masking the whole surface/texture as a
   * sprite list with a single sprite in it with
   * frameRect.x = 0 and frameRect.y = 0.
   * */
  setFrameRect(Rectangle(0,               // frameRect.x
                         0,               // frameRect.y
                         _imageWidth,     // frameRect.w
                         _imageHeight));  // frameRect.
}

void Text::copyTextContent(const char* text) {
  const size_t INPUT_TEXT_SIZE = strlen(text);

  // sanity check
  if (nullptr != _textContent) {
    delete[] _textContent;  // free current buffer
    _textContent = nullptr;
  }

  char* tempText = new char[INPUT_TEXT_SIZE + 1];  //+1 for the terminator
  strncpy(tempText, text, INPUT_TEXT_SIZE);

  // strncpy should be OK, but just to be sure
  tempText[INPUT_TEXT_SIZE] = '\0';

  _textContent = tempText;
}
