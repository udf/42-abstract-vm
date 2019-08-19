# abstract-vm
A stack based virtual machine with really really bad design choices

# overgrown rpn calculator

Ok so you can do maths on numbers like:

```
push int8(1)
push int8(1)
add
dump
```

which is a shitty way of doing `1 1 +` in a rpn calculator.


...and that's it, at least according to the project requirements.


# horrible type system
The machine supports different types (like int8, int16, etc).
When you do math on two different types the result is of the operand with the
highest precision.

Normally you'd use a union/variant for this, but you know what's cooler?
Abstract classes. "OOP" is the best thing ever.

(my operand class literally wraps a variant,
when i could just use a variant directly)

The really good thing about abstract classes is that you have to represent the
subclasses as a pointer to the abstract class.
This means that the actual operand needs to be stored somewhere else, aka the heap.

THIS MAKES THE MACHINE SLOWER THAN A CRIPPLE ON ROLLERSKATES. WHY THE FUCK WOULD
YOU JUST NOT LET US USE A UNION DIRECTLY AAAAAAAAAAAAAAAAAAAAA


# shitty tokenizer
Usually tokens are extracted using some sort of pattern matcher.
The easiest pattern matcher to use is regex, so that's what I do.

This means that for every token to be extracted we have to run a regex match for
a pattern for each possible token. As you can imagine, this is really fast.


# turing completeness
An overgrown rpn calculator is no way near turing complete, but we can fix this
with some simple additions. Namely, random memory access and loops.

RAM could be achieved by either:
- having two stacks
- having stack rotations

I do the latter because it's easier. This means that the stack is actually a
linked list. Which is faster than a traditional stack for rotation operations,
but slower at everything else.

## loops
Everyone loves a good conditional jump. That's why we have 6 conditional jumps.
Standard stuff really, they're just binary operations that only remove the top
of the stack.

### labels are a runtime construct
Because of the interactive mode, we can't know if a label exists until we
actually execute a jump instruction. So jumps are lazy. They're implemented as a
hash table. This is a good idea. Trust me.