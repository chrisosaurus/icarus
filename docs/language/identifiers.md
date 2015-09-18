# Identifiers

## Naming
All types must begin with either an uppercase letter (public) or an underscore followed by an uppercase letter (private)

All functions must begin with either a lowercase letter (public) or an underscore followed by a lower case letter (private)

In Icarus there are no global variables,
all local variables and argument must have a name beginning with a lowercase letter
## Shadowing

In Icarus a variable/type/function may never be given the same name as another within a scope, this is to prevent one identifier being hidden or shadowed behind another

This is illegal as the function and local variable have the same name

    fn foo()
        let bar = 5
    end

    fn bar()
    end


