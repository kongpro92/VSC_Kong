#ifndef BLOCKING_H__
#define BLOCKING_H__
#include "boundary.h"


class Block
{
public:
	Block() {}
	Block(Boundary bd_);
	~Block() {}

	Boundary bd;

private:

};


#endif // !BLOCKING_H__
