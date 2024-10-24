# RingBuffers
# DRE 2024 - Philosophicaldaemon@gmail.com

#Keywords: C, ringbuffer, ring buffer 

#Background: Consult the Wikipedia page: https://en.wikipedia.org/wiki/Circular_buffer for an introduction

#Introduction:

This repository holds many implementations of the idea of ringbuffers. It includes test cases that show operation as is, but one can make quick changes to make things reusable faster.

#Synopsis:
Here's Wikipedia's Uses explanation:

"Uses

The useful property of a circular buffer is that it does not need to have its elements shuffled around when one is consumed. (If a non-circular buffer were used then it would be necessary to shift all elements when one is consumed.) 
In other words, the circular buffer is well-suited as a FIFO (first in, first out) buffer while a standard, non-circular buffer is well suited as a LIFO (last in, first out) buffer.
Circular buffering makes a good implementation strategy for a queue that has fixed maximum size. Should a maximum size be adopted for a queue, then a circular buffer is a completely ideal implementation; all queue operations are constant time. 
However, expanding a circular buffer requires shifting memory, which is comparatively costly. 
For arbitrarily expanding queues, a linked list approach may be preferred instead.

In some situations, overwriting circular buffer can be used, e.g. in multimedia. If the buffer is used as the bounded buffer in the producer–consumer problem then it is probably desired for the producer (e.g., an audio generator) to overwrite old data if 
the consumer (e.g., the sound card) is unable to momentarily keep up. Also, the LZ77 family of lossless data compression algorithms operates on the assumption that strings seen more recently in a data stream are more likely to occur soon in the stream. 
Implementations store the most recent data in a circular buffer. "

This repository holds documented C code only.

#IDEA: how to use and re-use memory previously allocated over and over in an efficient manner that helps your code. 

Concept: changing the organization of the basic types and structs to make that idea achievable, optimizable, and modifiable to fit your needs easily. 

#How to make: use the autotools installed with the code :
autoconf, automake, libtools, 

# github Makefile recipes: This repository comes included with git commands as Makefile recipes in order to allow novice github users can contribute easier. 
Look in Makefile.am for the recipes.
