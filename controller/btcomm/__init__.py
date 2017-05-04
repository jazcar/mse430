from sys import platform


__all__ = ['btcomm']

if platform == 'linux':
    from btcomm.linux_socket import btcomm
elif platform == 'darwin':
    from btcomm.macos_serial import btcomm
else:
    raise ImportError('No bluetooth support for platform {}'.format(platform))
