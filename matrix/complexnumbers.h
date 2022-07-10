#pragma once
#include"pch.h"
template<class T>
class Comp {
public:
	T x;
	T y;
public:
	Comp() { x = y = 0; }
	template<class C>Comp(const Comp<C>&rhs) { x = rhs.x; y = rhs.y; }
	template<class C>Comp(C real, C imaginary = 0) { x = real; y = imaginary; }
	Comp(const D2D1_SIZE_F&rhs) { y = rhs.height; x = rhs.width; }
	Comp(const D2D1_POINT_2F&rhs) { y = rhs.y; x = rhs.x; }
	Comp operator()(T real, T imaginary = 0) { x = real; y = imaginary; return*this; }
	Comp operator=(const Comp&rhs) { x = rhs.x; y = rhs.y; return *this; }
	Comp operator*=(const Comp&rhs) {
		T bPart = x * rhs.x - y * rhs.y;
		y = y * rhs.x + x * rhs.y;
		x = bPart;
		return*this;
	}
	Comp operator*(const Comp&rhs) const{
		Comp buffer(x, y);
		buffer *= rhs;
		return buffer;
	}
	Comp operator+=(const Comp&rhs) { x += rhs.x; y += rhs.y; return*this; }
	Comp operator+(const Comp&rhs) const{ Comp buffer(x, y); buffer += rhs; return buffer; }
	Comp operator/=(const Comp&rhs) {
		T bPart = x / rhs.x - y / rhs.y;
		y = y / rhs.x + x / rhs.y;
		x = bPart;
		return*this;
	}
	Comp operator/(const Comp&rhs) const{ Comp buffer(x, y); buffer /= rhs; return buffer; }
	Comp operator-=(const Comp&rhs) { x -= rhs.x; y -= rhs.y; return*this; }
	Comp operator-(const Comp&rhs) const{ Comp buffer(x, y); buffer -= rhs; return buffer; }
	Comp operator*=(const T&rhs) { x *= rhs; y *= rhs; return*this; }
	Comp operator*(const T&rhs) const{ Comp buffer(x, y); buffer *= rhs; return buffer; }
	Comp operator/=(const T&rhs) { x /= rhs; y /= rhs; return*this; }
	Comp operator/(const T&rhs) const{ Comp buffer(x, y); buffer /= rhs; return buffer; }

	bool operator==(const Comp&rhs) const{ if (x == rhs.x&&y == rhs.y)return 1; return 0; }
	bool operator!=(const Comp&rhs) const{ return !(*this == rhs); }

	bool operator>(const Comp&rhs) const{ return x > rhs.x&&y > rhs.x; }
	bool operator<(const Comp&rhs) const{ return x < rhs.x&&y < rhs.x; }
	bool operator>=(const Comp&rhs) const{ return x >= rhs.x&&y >= rhs.x; }
	bool operator<=(const Comp&rhs) const{ return x <= rhs.x&&y <= rhs.x; }
public:
	void clear() { x = y = 0; }
public:
	double getD() const{//get distance to zero
		return sqrt(x*x + y*y);
	}
	double getA() const{//get Angle to x-axis
		return atan2(y , x);
	}
	D2D1_POINT_2F get() const{ return{ (float)x,(float)y }; }
	D2D1_SIZE_F getS() const { return{ (float)x,(float)y }; }
};
template<class T>Comp<T> CCompbyA(T distance, float Angel){
	Comp<T>buffer(cos(Angel*PI/180.0)*distance, sin(Angel*PI/180.0)*distance);
	return buffer;
}
template<class T>Comp<T> CCompbyR(T distance, float Radian) {//e^xi=cos(x)+isin(x)
	Comp<T>buffer(cos(Radian)*distance, sin(Radian)*distance);
	return buffer;
}