#ifndef _LOADABLEMANAGER_HPP_
#define _LOADABLEMANAGER_HPP_

#include <map>
#include <string>

#include "font.hpp"

/*since we want to store and recycle heavy loadable object (textures, fonts) we use this class to manage them*/

template<class T>
class LoadableManager {
    private:
        typedef std::pair<std::string, T> loadable_pair;
        std::map<std::string, T> loadable_map; //the map where we will store our resources
        T default_resource; //if no resource has been found we will return this

        T& loadResource(const std::string& thePath); //if the resource has not been already loaded, we load it

    public:
        T& getResource(const std::string& theID);
};

template<class T>
T& LoadableManager<T>::getResource(const std::string& theID) {
    auto it = loadable_map.find(theID);

    if( it != loadable_map.end() ) return it->second;
    else return loadResource(theID);
}

template<class T>
T& LoadableManager<T>::loadResource(const std::string& thePath) {
    T buffer;
    if( buffer.loadFromFile(thePath) )
        return loadable_map.insert(std::make_pair(thePath, buffer)).first->second;

    else return default_resource;
}

typedef LoadableManager<Font> FontManager;

#endif // _LOADABLEMANAGER_HPP_
