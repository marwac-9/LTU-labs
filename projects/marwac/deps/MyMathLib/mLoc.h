namespace mwm
{
/*! struct loc used for indexing of matrix */
struct loc
{
	int x;
	int y;
	/*! \fn constructor with initialization list*/
	loc(int x, int y) : x(x), y(y) {}
};
}
