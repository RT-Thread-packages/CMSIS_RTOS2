from building import *

src	= Glob('*.c')
cwd = GetCurrentDir()

path = [cwd]
group = DefineGroup('CMSIS-RTOS2', src, depend = ['PKG_USING_CMSIS_RTOS2'], CPPPATH = path)
Return('group')
