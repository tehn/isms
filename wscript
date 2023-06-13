import shutil
from pathlib import Path
from waflib.Build import BuildContext

class install_lib_class(BuildContext):
    cmd = 'install-lib'
    fun = 'install_lib'

def options(opt):
    opt.load('compiler_c compiler_cxx')

def configure(conf):
    conf.load('compiler_c compiler_cxx')

    try:
        conf.check_cfg(package='lua54', args='--cflags --libs', uselib_store='LUA')
    except conf.errors.ConfigurationError:
        conf.check_cfg(package='lua', args='lua >= 5.4 --cflags --libs', uselib_store='LUA')

    conf.check_cfg(msg='Checking for SDL2',
        package='',
        path='sdl2-config',
        args='--cflags --libs',
        uselib_store='SDL2')

    conf.check_cfg(package='liblo', args='--cflags --libs', uselib_store='LO')
    conf.check_cc(lib='pthread', uselib_store='PTHREAD')
    conf.check_cc(lib='m', uselib_store='M')

    if conf.env.DEST_OS == "linux":
        try:
            conf.check_cfg(package='alsa', args='--cflags --libs', uselib_store='ALSA')
            conf.env.append_value('RTMIDI_DEFINES', '__LINUX_ALSA__')
            conf.env.append_value('RTMIDI_USE', 'ALSA')
        except conf.errors.ConfigurationError:
            pass

def build(ctx):
    ctx.stlib(features='cxx',
        source=['rtmidi/rtmidi_c.cpp', 'rtmidi/RtMidi.cpp'],
        defines=ctx.env.RTMIDI_DEFINES,
        target='rtmidi')

    ctx.program(features='cxx',
        source=ctx.path.ant_glob('src/**/*.c'),
        includes=['src', 'src/clock', 'rtmidi'],
        target='isms',
        use=['LO', 'LUA', 'SDL2', 'PTHREAD', 'M', 'rtmidi'] + ctx.env.RTMIDI_USE)

def install_lib(ctx):
    src = Path(ctx.path.abspath()) / 'lua'
    dst = Path.home() / '.local' / 'share' / 'isms' / 'system'

    dst.mkdir(parents=True, exist_ok=True)
    shutil.copytree(src, dst, dirs_exist_ok=True)
