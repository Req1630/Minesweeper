#ifndef SINGLETON_BASE_H
#define SINGLETON_BASE_H

#include "..\Global.h"

using namespace std;

template <class T>
class CSingletonBase
{
public:
	static T* GetInstance() {
		static unique_ptr<T> pInstance = make_unique<T>();
		return pInstance.get();
	}

protected:
	// 継承用のコンストラクタ・デストラクタ.
	CSingletonBase(){};
	virtual ~CSingletonBase() {}

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSingletonBase( const CSingletonBase & ) = delete;
	CSingletonBase& operator = ( const CSingletonBase & ) = delete;
	CSingletonBase( CSingletonBase && ) = delete;
	CSingletonBase& operator = ( CSingletonBase && ) = delete;
};

#endif	// #ifndef SINGLETON_BASE_H.