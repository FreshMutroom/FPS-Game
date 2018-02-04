# FPS-Demo

![](https://i.imgur.com/pJ6K1iL.jpg)

A FPS created with Unreal Engine 4.

A 12 minute video of gameplay can be watched [**here**](https://goo.gl/z7kRWe)

**WARNING:** Before downloading the game you must note that I can't guarantee all sounds will be at correct levels. Because of this you should either
- play with your OS sound muted
- play with your OS sound low (say at 10%) and adjust the in-game volume until it is comfortable

After reading the warning above you can download the game for Windows [**here**](https://goo.gl/2w85Sh) (2GB)

---

**What to expect from this game:**
- 4 months worth of a game from working with Unreal Engine 4
- 100% free assets
- All art assets not created by me (excluding some basic things like crosshairs and decals)
- Everything implemented by me from reading/watching tutorials - nothing is just copied and pasted (There is one exception here and that is the material used for creating a yellow highlight around meshes; I have borrowed this without bothering to figure out how it works)
- Saving and loading to not work
- Focus more on implementation than a pretty looking environment
- A game nowhere near 100% complete

**What to expect when looking at the source code:**
- Code written with only myself in mind
- Plenty of white space
- Lots of ```if``` statements for ```nullptr``` checks which should use a helper function instead
- Very little use of the keyword ```const```

**Additional info:**
- Made mostly with UE4.15 but I switched to 4.17 right before packaging since there was something in 4.15 preventing it from working.
- I used blueprints early on. Those things I did implement in blueprints I did not convert to C++, but near the end I tried to use C++ as much as possible.
