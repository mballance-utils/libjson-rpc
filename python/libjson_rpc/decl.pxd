
cimport debug_mgr.decl as dm

from libcpp.string cimport string as cpp_string
from libcpp.vector cimport vector as cpp_vector
from libcpp.memory cimport unique_ptr
from libcpp cimport bool
from libc.stdint cimport int32_t
cimport cpython.ref as cpy_ref

ctypedef IFactory *IFactoryP

cdef extern from "jrpc/IFactory.h" namespace "jrpc":
    cdef cppclass IFactory:
        void init(dm.IDebugMgr *dmgr)



