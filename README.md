Carelessnes is not a crime!
===========================

Motivation
----------

Have you ever been too careless when doing `rm`? Have you ever lost important
data? You certainly have and even if you haven't then it's very likely that
it'll soon happen to you!

The motto of the program is this: necessity is the mother of all invention
(i.e. I've just deleted some pretty important files).

Usage
-----

Careless `rm` is pretty simple. Just run

```
careless rm -rf very_important_directory
```

The directory is gone now! However you can bring it back to life. Examine your
last careless operations by issuing

```
careless log
4883.1368714872 /home/directory rm -rf very_important_directory
```

There are three columns: the ID of your careless action, the directory in which
it took place and the action itself. In order to revert the action issue

```
careless revert 4883.1368714872
```

You'll find the deleted directory back in its place! If you want to revert the
last operation then run

```
careless revert last
```

That's it.

The most important fact is that *the careless version supports exactly the
same arguments as the original command*. It uses `rm` under the hood.

Limitations
-----------

- only `rm` is currently supported; other possibly destructive operations are
  `cp` and `mv`
- only whole operation can be reverted, not individual files
- the reverted files are placed in their original locations
- `revert last` may throw an error when there are no operations logged
- support only the partition that contains the home directory
