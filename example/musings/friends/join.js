
# type dependent implementation
fn join<T>(a::T, b::T) -> T
    ...
end

# str + list of str => str
fn join<T>(t::T, list::Iterable<T>) -> T
    let progress = T()
    for item in list
        progress = progress.join(item)
        if not last
            progress = progress.join(t)
        end
    end
    return progress
end

# more complex
# need a way of expressing that we can convert a U to a T
fn join<T, U>(t::T, list::Iterable<U>) -> T
    let progress = T()
    for item in list
        progress = progress.join(convert<U,T(item))
        if not last
            progress = progress.join(t)
        end
    end
    return progress
end