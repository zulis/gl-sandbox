#pragma once

template <typename T>
class Singleton
{
public:
	template <typename... Args>
	static T* get(Args... args)
	{
		if (!mIinstance)
		{
			mIinstance = new T(std::forward<Args>(args)...);
		}

		return mIinstance;
	}

	static void destroy()
	{
		delete mIinstance;
		mIinstance = nullptr;
	}

private:
	static T* mIinstance;
};

template <class T> T*  Singleton<T>::mIinstance = nullptr;

/*
class Map: public Singleton<Map>
{
friend class Singleton<Map>;
private:
Map(int size_x, int size_y): size_x_{size_x}, size_y_{size_y} {}

public:
int size_x_;
int size_y_;
};

int main()
{
Map* m = Map::get(4, 5);

std::cout << m->size_y_ << std::endl; // Outputs 5.

Map::destroy();
}
*/