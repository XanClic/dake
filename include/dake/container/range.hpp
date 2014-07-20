#ifndef DAKE__CONTAINER__RANGE_HPP
#define DAKE__CONTAINER__RANGE_HPP

#include <iterator>


namespace dake
{
namespace container
{

template<typename T> class range;

template<typename T>
class range_iterator
{
    private:
        T pos;

        range_iterator(const T &initial):
            pos(initial)
        {}

    public:
        typedef T difference_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        typedef std::bidirectional_iterator_tag iterator_category;


        T operator*(void)
        { return pos; }

        T *operator->(void)
        { return &pos; }

        bool operator!=(const range_iterator &ri)
        { return ri.pos != pos; }

        range_iterator &operator++(void)
        { ++pos; return *this; }

        range_iterator operator++(int _)
        { (void)_; range_iterator<T> ri(pos); ++pos; return ri; }

        range_iterator &operator--(void)
        { --pos; return *this; }

        range_iterator operator--(int _)
        { (void)_; range_iterator<T> ri(pos); --pos; return ri; }


    friend class range<T>;
};


template<typename T = int>
class range
{
    private:
        T s, e;

    public:
        range(const T &st, const T &ed):
            s(st), e(ed)
        { ++e; }

        range_iterator<T> begin(void) const
        { return range_iterator<T>(s); }

        range_iterator<T> end(void) const
        { return range_iterator<T>(e); }


    friend class range_iterator<T>;
};

}
}

#endif
