/*
 * Copyright 2012-2013 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../Engine/Animation.h"
#include "../Engine/ResourceManager.h"
#include <iostream>

namespace Falltergeist
{

Animation::Animation(const char * filename, int x, int y) : InteractiveSurface(0, 0, x, y)
{
    _surfaceSets = new std::vector<std::vector<Surface *> *>;
    _currentFrame = 0;
    _currentSurfaceSet = 0;
    _frameRate = 400;
    _lastTicks = SDL_GetTicks();
    loadFromFrmFile(filename);
}

Animation::~Animation()
{
    delete _surfaceSets;
}

void Animation::think()
{
    if(_lastTicks + _frameRate > SDL_GetTicks()) return;
    _lastTicks = SDL_GetTicks();
    _currentFrame++;
    if (_currentFrame >= _surfaceSets->at(_currentSurfaceSet)->size())
    {
        _currentFrame = 0;
    }
}

void Animation::loadFromFrmFile(const char * filename)
{
    libfalltergeist::FrmFileType * frm = ResourceManager::getFrmFileType(filename);
    if (!frm)
    {
        std::cout << "can't find FRM file " << filename << std::endl;
    }
    libfalltergeist::PalFileType * pal = ResourceManager::getPalFileType("color.pal");

    _frameRate = 1000 / frm->getFramesPerSecond();

    // for each direction
    for (unsigned int i = 0; i != 6; ++i)
    {
        if (i > 0 && frm->getDirections()->at(i)->getDataOffset() == frm->getDirections()->at(0)->getDataOffset()) break;
        std::vector<Surface *> * frameset = new std::vector<Surface *>;
        // for each frame
        for (unsigned int j = 0; j != frm->getFramesPerDirection(); ++j)
        {
            int width = frm->getDirections()->at(i)->getFrames()->at(j)->getWidth();
            int height = frm->getDirections()->at(i)->getFrames()->at(j)->getHeight();
            Surface * surface = new Surface(width,height);
            int z = 0;
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    // 12 - frame data offset
                    unsigned int colorIndex = frm->getDirections()->at(i)->getFrames()->at(j)->getColorIndexes()->at(z);
                    unsigned int color = *pal->getColor(colorIndex);
                    if (colorIndex == 0) color = 0;
                    surface->setPixel(x,y,color);
                    z++;
                }
            }
            frameset->push_back(surface);
        }
        _surfaceSets->push_back(frameset);
    }
}

SDL_Surface * Animation::getSurface()
{
    if (needRedraw)
    {
        draw();
        needRedraw = false;
    }
    return _surfaceSets->at(_currentSurfaceSet)->at(_currentFrame)->getSurface();
}


}
