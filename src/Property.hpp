#ifndef _PROPERTY_HPP
#define _PROPERTY_HPP

#include <vector>
#include <algorithm>
#include "particles/PropertyHandle.hpp"
#include "particles/ParticleHandle.hpp"
#include "particles/Storage.hpp"
#include "Particle.hpp"

namespace espresso {
  /** Exception thrown when a property and a storage don't match. */
  class StorageMismatch : public std::exception 
  {};

  class PropertyBase {
  public:
    typedef shared_ptr< PropertyBase > SelfPtr;

    PropertyBase(const particles::Storage::SelfPtr _storage);

    void checkFitsTo(const particles::Set::SelfPtr set) const;


  protected:
    const particles::Storage::SelfPtr getStorage() const { return storage; }

  private:
    // what storage does the property belong to
    const particles::Storage::SelfPtr storage;
  };

  /** Scalar particle property. This does several things:
      <ul>
      <li> Lifetime management of the property. Deletion
      of the Property object removes the property from
      its storage.
      <li> Slow, but direct access to the particle properties
      without the need for handles.
      </ul>

      @tparam T type stored in the property
  */
  // TODO: Should be noncopyable!
  //  class Property :  boost::noncopyable {
  template < typename T >
  class Property : public PropertyBase {
    typedef particles::PropertyHandle< T > Handle;
  public:
    typedef shared_ptr< Property< T > > SelfPtr;

    // visible in Python
    Property(particles::Storage::SelfPtr _storage)
      : PropertyBase(_storage), 
	id(getStorage()->template addProperty<T>())
    {}

    ~Property() {
      getStorage()->deleteProperty(id);
    }

    T getItem(ParticleId part) {
      return (*this).at(part);
    }

    void setItem(ParticleId part, const T &v) {
      (*this).at(part) = v;
    }

  public: // invisible in Python
    Handle getHandle(particles::Set::SelfPtr set) {
      checkFitsTo(set);
      return getHandle();
    }

    T &at(particles::Set::SelfPtr set, particles::ParticleHandle particle) {
      checkFitsTo(set);
      return getHandle()[particle];
    }

    T &operator[](ParticleId id) {
      particles::ParticleHandle particle =
        getStorage()->getParticleHandle(id);
      return getHandle()[particle];
    }

    /// checked access
    T &at(ParticleId id) {
      particles::ParticleHandle particle =
        getStorage()->getParticleHandle(id);
      if (particle == particles::ParticleHandle()) {
	throw std::out_of_range("Property::at");
      }
      return getHandle()[particle];
    }

  private:
    Handle getHandle() {
      return getStorage()->template getPropertyHandle< T >(id);
    }

    esutil::TupleVector::PropertyId id;
  };


  /** Array particle property. This does several things:
      <ul>
      <li> lifetime management of the property. deletion
      of the Property object removes the property from
      its storage.
      <li> slow, but direct access to the particle properties
      without the need for handles.
      </ul>

      @tparam T type stored in the property
  */
  template < typename T >
  class ArrayProperty : public PropertyBase {
    typedef particles::ArrayPropertyHandle<T> Handle;

  public: // visible in Python
    typedef shared_ptr< ArrayProperty< T > > SelfPtr;

    std::vector<T> getItem(ParticleId part) {
      Handle ref = getHandle();
      const T
        *start = (*this).at(part),
        *end   = start + ref.getDimension();
      return std::vector<T>(start, end);
    }

    void setItem(ParticleId part, const std::vector<T> &v) {
      Handle ref = getHandle();
      if (v.size() != ref.getDimension())
        throw std::range_error("ArrayProperty::setItem: incorrect dimension");
      std::copy(v.begin(), v.end(), (*this).at(part));
    }

  public: // invisible in Python
    ArrayProperty(particles::Storage::SelfPtr _storage,
                  size_t dimension)
      : PropertyBase(_storage), 
	id(_storage->template addProperty<T>(dimension))
    {}

    ~ArrayProperty() {
      getStorage()->deleteProperty(id);
    }

    Handle getHandle(particles::Set::SelfPtr set) {
      checkFitsTo(set);
      return getHandle();
    }

    T *at(particles::Set::SelfPtr set, particles::ParticleHandle handle) {
      checkFitsTo(set);
      return getHandle()[handle];
    }

    T *operator[](ParticleId part) {
      particles::ParticleHandle particle =
        getStorage()->getParticleHandle(part);
      return getHandle()[particle];
    }

    /// checked access
    T *at(ParticleId part) {
      particles::ParticleHandle particle =
        getStorage()->getParticleHandle(part);
      if (particle == particles::ParticleHandle()) {
	throw std::out_of_range("ArrayProperty::at");
      }
      return getHandle()[particle];
    }

  private:
    Handle getHandle() {
      return getStorage()->template getArrayPropertyHandle<T>(id);
    }

    esutil::TupleVector::PropertyId id;
  };

  void registerPythonProperty();

}

#endif
