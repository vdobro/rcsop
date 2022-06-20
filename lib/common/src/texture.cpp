#include "texture.h"

#include <utility>

Texture::Texture(path file_path,
                 const ObserverPosition& position)
        : _file_path(std::move(file_path)),
          _camera_position(position) {}

ObserverPosition Texture::position() const {
    return this->_camera_position;
}

path Texture::file_path() const {
    return this->_file_path;
}
