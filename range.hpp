//
// Created by rtorres12 on 11/18/15.
// For template classes just delcare them fully in the header file..fuck it...
template <typename T>
class range {
private:
    class iter {
    private:
        T m_x;

    public:
        iter(T x) : m_x(x) { }

        void operator++() { m_x++; }

        const T& operator*() const { return m_x; }

        bool operator!=(const iter &other) const {
            return m_x != other.m_x;
        }
    };

    T m_begin;
    T m_end;

public:
    iter begin() const { return iter(m_begin); }
    iter end()   const { return iter(m_end  ); }

    range(T range_begin, T range_end)
            : m_begin(range_begin), m_end(range_end)
    { }
};