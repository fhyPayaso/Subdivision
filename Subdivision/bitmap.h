/*
Bitmap.h

Copyright (C) 2002-2005 Ren�� Nyffenegger

This source code is provided 'as-is', without any express or implied
warranty. In no event will the author be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this source code must not be misrepresented; you must not
claim that you wrote the original source code. If you use this source code
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original source code.

3. This notice may not be removed or altered from any source distribution.

Ren�� Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/


#ifndef BITMAP_H_
#define BITMAP_H_

#include <string>
#include <sstream>
#include <windows.h>

class Bitmap {
public:
	Bitmap();
	Bitmap(std::string const& file_name);

	operator HBITMAP() const;

protected:
	friend class MemoryDC;
	Bitmap(HBITMAP);
	HBITMAP bitmap_;
};

#endif