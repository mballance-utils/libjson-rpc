
import ctypes
cimport debug_mgr.core as dm
cimport libjson_rpc.decl as jrpc
from libcpp cimport bool
from libc.stdint cimport int32_t

cdef class Factory(object):
    cdef jrpc.IFactory      *_hndl

    cpdef void init(self, dm.DebugMgr dmgr)
    pass



