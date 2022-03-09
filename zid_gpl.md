# Working From Home with ZID-GPL

All programming exercise solutions you hand in **must** compile properly on the [ZID-GPL server](https://www.uibk.ac.at/zid/systeme/linux/lpccs_4/benutzeranleitung_zid-gpl.html).
The easiest way of ensuring this is by developing on ZID-GPL directly, either through a terminal or using an editor/IDE with support for remote development via SSH (such as [Visual Studio Code](https://code.visualstudio.com/docs/remote/ssh)).

Note that the default version of the GCC compiler present on ZID-GPL is _extremely_ outdated.
You may load a newer version by calling `module load gcc/9.2.0`.

It appears as there is a bug where man pages cannot be viewed after loading a module.
This has been reported to ZID and be fixed (hopefully) soon.
In the meantime please use the following command to work around this issue:

`export MANPATH=":$MANPATH"`

## Connecting to ZID-GPL via SSH

**NOTE**: You have to be inside the University network to be able to connect
via SSH. See [here](https://www.uibk.ac.at/zid/netz-komm/vpn/) for more
information.

To log into ZID-GPL via SSH, use the following command on Linux/Mac OSX:

`ssh -l <username> zid-gpl.uibk.ac.at`

On Windows, you can use Powershell (Windows 10) or a third-party SSH client
such as [PuTTY](https://www.putty.org/).
