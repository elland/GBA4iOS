////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2009 Laurent Gomila (laurent.gom@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#ifdef _MSC_VER
    #pragma warning(disable : 4127) // "conditional expression is constant" generated by the FD_SET macro
#endif

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Network/SelectorBase.hpp>


namespace sf
{
////////////////////////////////////////////////////////////
/// Default constructor
////////////////////////////////////////////////////////////
SelectorBase::SelectorBase() :
myMaxSocket(0)
{
    Clear();
}


////////////////////////////////////////////////////////////
/// Add a socket to watch
////////////////////////////////////////////////////////////
void SelectorBase::Add(SocketHelper::SocketType Socket)
{
    FD_SET(Socket, &mySet);

    int Size = static_cast<int>(Socket);
    if (Size > myMaxSocket)
        myMaxSocket = Size;
}


////////////////////////////////////////////////////////////
/// Remove a socket
////////////////////////////////////////////////////////////
void SelectorBase::Remove(SocketHelper::SocketType Socket)
{
    FD_CLR(Socket, &mySet);
}


////////////////////////////////////////////////////////////
/// Remove all sockets
////////////////////////////////////////////////////////////
void SelectorBase::Clear()
{
    FD_ZERO(&mySet);
    FD_ZERO(&mySetReady);

    myMaxSocket = 0;
}


////////////////////////////////////////////////////////////
/// Wait and collect sockets which are ready for reading.
/// This functions will return either when at least one socket
/// is ready, or when the given time is out
////////////////////////////////////////////////////////////
unsigned int SelectorBase::Wait(float Timeout)
{
    // Setup the timeout structure
    timeval Time;
    Time.tv_sec  = static_cast<long>(Timeout);
    Time.tv_usec = (static_cast<long>(Timeout * 1000) % 1000) * 1000;

    // Prepare the set of sockets to return
    mySetReady = mySet;

    // Wait until one of the sockets is ready for reading, or timeout is reached
    int NbSockets = select(myMaxSocket + 1, &mySetReady, NULL, NULL,  /* Timeout > 0 ? &Time : NULL */ &Time); // Changed so a 0 Timeout value returns immediately for polling purposes

    return NbSockets >= 0 ? static_cast<unsigned int>(NbSockets) : 0;
}


////////////////////////////////////////////////////////////
/// After a call to Wait(), get the Index-th socket which is
/// ready for reading. The total number of sockets ready
/// is the integer returned by the previous call to Wait()
////////////////////////////////////////////////////////////
SocketHelper::SocketType SelectorBase::GetSocketReady(unsigned int Index)
{
    // The standard FD_xxx interface doesn't define a direct access,
    // so we must go through the whole set to find the socket we're looking for
    for (int i = 0; i < myMaxSocket + 1; ++i)
    {
        if (FD_ISSET(i, &mySetReady))
        {
            // Current socket is ready, but is it the Index-th one ?
            if (Index > 0)
            {
                Index--;
            }
            else
            {
                return static_cast<SocketHelper::SocketType>(i);
            }
        }
    }

    // Invalid index : return an invalid socket
    return SocketHelper::InvalidSocket();
}

} // namespace sf