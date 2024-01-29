
import os
import ctypes
from libcpp.vector cimport vector as cpp_vector
from libc.stdint cimport intptr_t
cimport debug_mgr.core as dm_core
cimport libjson_rpc.decl as decl

_inst = None

cdef class Factory(object):
    
    def __init__(self):
        pass

    cpdef void init(self, dm.DebugMgr dmgr):
        self._hndl.init(dmgr._hndl)

    @staticmethod
    def inst():
        cdef decl.IFactory *hndl = NULL
        cdef Factory factory
        global _inst

        if _inst is None:
            ext_dir = os.path.dirname(os.path.abspath(__file__))

            build_dir = os.path.abspath(os.path.join(ext_dir, "../../build"))

            # First, look in the build directory
            core_lib = None

            # TODO: check already-loaded libraries for libdebug-mgr symbols?
            libname = "libjson-rpc.so"
            # for libdir in ("lib", "lib64"):
            #     if os.path.isfile(os.path.join(build_dir, libdir, libname)):
            #         core_lib = os.path.join(build_dir, libdir, libname)
            #         break
            if core_lib is None:
                core_lib = os.path.join(ext_dir, libname)

            if not os.path.isfile(core_lib):
                raise Exception("Extension library core \"%s\" doesn't exist" % core_lib)

            print("Loading library %s" % core_lib)
            so = ctypes.cdll.LoadLibrary(core_lib)

            func = so.jrpc_getFactory
            func.restype = ctypes.c_void_p

            hndl_v = func()
            hndl = <decl.IFactoryP>(<intptr_t>(hndl_v))

            factory = Factory()
            factory._hndl = hndl
            dmgr_f = dm_core.Factory.inst()

            # Initialize factory
            factory.init(dmgr_f.getDebugMgr())
            _inst = factory

        return _inst


