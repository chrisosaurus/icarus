Enforcing constraints
=====================

some musings about enforcing constraints and various approaches, nothing here is even remotely close to planned.

starting point
--------------

It is sometimes useful to be able to hide information for the purposes of enforcing constraints


    type Date
        _day   :: Sint
        _month :: Sint
        _year  :: Sint

        Date(d::Sint, m::Sint, y::Sint)
            # naive first solution
            assert(ordered(0,    d, 32))
            assert(ordered(0,    m, 13))
            assert(ordered(1900, y, 2100))

            return new(d, m, y)
        end
    end


removing boiler plate
---------------------

However we could also handle this instead with public fields with constraints on their value

    type Date
        day   :: Sint :: ordered(0,    day,   32)
        month :: Sint :: ordered(0,    month, 13)
        year  :: Sint :: ordered(1900, year,   2100)
    end


Of course in this case the above constrains do not have the ability to enforce things such as

 - February usually has 28 days, 29 on leap years
 - March has 31 days
 - June has 30 days


getting smarter
---------------

here is the version with a constructor, getter and setters

    type Date
        _day   :: Sint
        _month :: Sint
        _year  :: Sint

        Date(d::Sint, m::Sint, y::Sint)
            assert(validate_date(d,m,y))
            return new(d,my,y)
        end
    end

    fn validate_date(d::Sint, m::Sint, y::Sint) -> Bool
        if m == 2
            # february
            if is_leap_year(y)
                assert(ordered(0, d, 30))
            else
                assert(ordered(0, d, 29))
            end
        else if m in (1, 3, 5, 7, 8, 10, 12)
            # january, march, may, july, august, october, december have 31 days
            assert(ordered(0, d, 32))
        else
            # everything else has 30
            assert(ordered(0, d, 31))
        end
    end

    fn day(date::Date) -> Sint
        date._day
    end

    fn day(&date::Date, d::Sint)
        assert(validate_date(d, date._month, date._year))
        &date._day = d
    end

    fn month(date::Date) -> Sint
        date._month
    end

    fn year(&date::Date, m::Sint)
        assert(validate_date(date._day, m, date._year))
        &date._month = m
    end

    fn year(date::Date) -> Sint
        date._year
    end

    fn year(&date::Date, y::Sint)
        assert(validate_date(date._day, date._month, y))
        &date._year = y
    end


But this is getting very tedious and error prone, this also means we now have to treat this Date object differently to a POD type:

    let &d = Date(2, 7, 2014)
    &d.year(2015)
    printf d.day()


even smarter
------------

we could try model this with something like

    type Date
        day   :: Sint :: validate_date(day, month, year)
        month :: Sint :: validate_date(day, month, year)
        year  :: Sint :: validate_date(day, month, year)
    end

    fn validate_date(d::Sint, m::Sint, y::Sint) -> Bool
        if m == 2
            # february
            if is_leap_year(y)
                assert(ordered(0, d, 30))
            else
                assert(ordered(0, d, 29))
            end
        else if m in (1, 3, 5, 7, 8, 10, 12)
            # january, march, may, july, august, october, december have 31 days
            assert(ordered(0, d, 32))
        else
            # everything else has 30
            assert(ordered(0, d, 31))
        end
    end


and then allow the syntax for accessing these fields to be simpler

    let &d = Date(2, 7, 2014)
    &d.year = 2015


and let the compiler enforce constraints, although how the assert indicates failure to this caller is still up for debate.

the biggest win here is the getter and setters are 'for free', the interface is far simpler and the same as a POD type.

this also allows for a simpler POD type to have constraints added on later (or removed) without having to change user code.



