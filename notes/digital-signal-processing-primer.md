Notes from 'A Digital Signal Processing Primer' by Ken Steiglitz.

# Ch. 1: Tuning Forks & Phasors

## Physics refresher
Displacement described by $F=ma=-kx$. In other words $x''=-(k/m)x$ or
$$ f(x) = \sin(\omega x) or \cos(\omega x) $$
$$ \omega = \sqrt{k/m} $$
- $\omega$ is frequency.
- Period of vibration is $2\pi/\omega$.
- Think guitar strings: heavier -> lower frequencies.

## Adding sinusoids
Adding sinusoids of same frequency => sum is the same frequency.
**Intuition**:
- Think of two vectors u, v representing rotating on a different circles, but at the same speed.
- Picture typical paralelogram draw u,v,u+v.
- Everything rotates as one piece, and u+v can be used to capture a new sinusoid of the same frequency.

Notice that the derivative vectors are always at right angles.

## Complex numbers
Think of multiplication by $i$ as a 90 degree rotation. Note that $j^2=-1$ reverses direction.

A circle can be represented by $cos(\omega t) + j \sin (\omega t)$.