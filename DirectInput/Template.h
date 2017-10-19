#pragma once

#ifndef  __TEMPLATE_H_
#define __TEMPLATE_H_

template<class T> void SafeRelease(T t)
{
	if(t)
	{
		t->Release();
		t = 0;
	}
}

template<class T> void SafeDelete(T t)
{
	if(t)
	{
		delete t;
		t = 0;
	}
}

#endif // !__TEMPLATE_H_
