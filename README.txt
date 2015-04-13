Add echos to stereo PCM sounds stored in the cononical wav format.


How is echo added? There are two important ideas to understand here:

Volume: multiplying each sample value by a number greater than 1 increases the volume; multiplying each sample by a number between 0 and 1 (exclusive) decreases the volume. For example, multiplying everything by 2 doubles the volume; multiplying everything by 0.5 halves the volume.
Mixing: if we mix two sounds together, what we mean is that they play at the same time. Mixing two sounds involves adding corresponding samples together. For example, if one sound has three samples: 2, 4, and 6; and another sound has four samples: 10, 11, 12, and 13; mixing them yields a sound of four samples: 12, 15, 18, and 13.