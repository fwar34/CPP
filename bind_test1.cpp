#include <iostream>
#include <tr1/functional>

using namespace std;
   
typedef std::tr1::function<void(int)> HandlerEvent;

struct Shape
{
	HandlerEvent event;	
};

class Rectangle
{
private:
	string m_name;
	Shape m_shape;
public:
	void init()
	{
		m_shape.event = bind(&Rectangle::onEvent, this, tr1::placeholders::_1);
	}

	const Shape getShape() const
	{
		return m_shape;
	}

	virtual void onEvent(int arg)
	{
		cout << "Rectangle::onEvent arg : " << arg << endl;
	}
};

class Square : public Rectangle
{
	void onEvent(int arg)
	{
		cout << "Square::onEvent arg : " << arg << endl;
	}
};

int main()
{
	Rectangle rect;
	rect.init();
	rect.getShape().event(333);

	Square squ;
	squ.init();
	squ.getShape().event(444);
	
	return 0;
}
