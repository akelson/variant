#include <iostream>

class Variant {
public:
  template <typename T>
  static Variant make(T val) {
    Variant v;
    v.mType = to_type_enum<T>::value;
    v.value<T>() = val;
    return v;
  }

  template <typename T>
  bool assignTo(T *dest) {
    if (to_type_enum<T>::value == mType) {
      *dest = value<T>();
      return true;
    } else {
      return false;
    }
  }

private:
  Variant() {}

  enum TypeEnum {INT, UINT, FLOAT};

  template <typename T>
  struct to_type_enum {
    static const TypeEnum value;
  };

  template <typename T>
  T &value();

  TypeEnum mType;

  union {
    int           mInt;
    unsigned int  mUInt;
    float         mFloat;
  };
}; // Variant

template<> const Variant::TypeEnum Variant::to_type_enum<int>::value = Variant::INT;
template<> const Variant::TypeEnum Variant::to_type_enum<unsigned int>::value = Variant::UINT;
template<> const Variant::TypeEnum Variant::to_type_enum<float>::value = Variant::FLOAT;

template<> int& Variant::value<int>() { return mInt; }
template<> unsigned int& Variant::value<unsigned int>() { return mUInt; }
template<> float& Variant::value<float>() { return mFloat; }

class CheckParams {
public:
  virtual bool setMin(Variant val) { return false; }
  virtual bool setMax(Variant val) { return false; }
};

class Check {
public:
  virtual bool eval() = 0;
  virtual CheckParams& getParams() = 0;
};

template <typename T>
class CheckParamsMin : public CheckParams {
public:
  virtual bool setMin(Variant val) {
    return val.assignTo(&mMin);
  }

  T mMin;
};

template <typename T>
class CheckParamsMax : public CheckParams {
public:
  virtual bool setMax(Variant val) {
    return val.assignTo(&mMax);
  }

  T mMax;
};

template <typename T>
class CheckParamsMinMax : public CheckParams {
public:
  virtual bool setMin(Variant val) {
    return val.assignTo(&mMin);
  }

  virtual bool setMax(Variant val) {
    return val.assignTo(&mMin);
  }

  T mMin;
  T mMax;
};



template <typename T>
class CheckLT : public Check {
public:
  CheckLT(T *valPtr) : mValPtr(valPtr) {}

  virtual bool eval() {
    return *mValPtr < mParams.mMax;
  }

  virtual CheckParams& getParams() {
    return mParams;
  }

private:
  CheckParamsMax<T> mParams;
  T *mValPtr;
};

template <typename T>
class CheckRange : public Check {
public:
  CheckRange(T *valPtr) : mValPtr(valPtr) {}

  virtual bool eval() {
    return (*mValPtr > mParams.mMin) && (*mValPtr < mParams.mMax);
  }

  virtual CheckParams& getParams() {
    return mParams;
  }

private:
  CheckParamsMinMax<T> mParams;
  T *mValPtr;
};

int main() {

  float foo = 41;
  Check *check = new CheckLT<float>(&foo);

  Variant v1 = Variant::make<int>(42);

  if (check->getParams().setMax(v1)) {
    std::cout << "set success" << std::endl;
  } else {
    std::cout << "set failure" << std::endl;
  }

  if (check->eval()) {
    std::cout << "success" << std::endl;
  } else {
    std::cout << "failure" << std::endl;
  }

  return 0;
}


