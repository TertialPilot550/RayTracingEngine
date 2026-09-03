# A Plain-English Companion to “From Primitives to a Depth Buffer”

## What this document is about

A graphics program starts with things that exist in a three-dimensional world: triangles, rectangles, spheres, and other shapes.

But a screen does not understand triangles or spheres. A screen only has a grid of pixels.

So a renderer has to answer a series of questions:

1. Where is the object in the world?
2. Where does the camera see it?
3. Where does it appear on the screen?
4. Which pixels does it cover?
5. What should each covered pixel look like?
6. Which object is actually in front when several objects cover the same pixel?

The original document focuses on the part between **“I have a geometric primitive”** and **“I know which pixels it affects and what depth and attributes those pixels have.”**

That process is called **rasterization**.

The most important idea to keep in mind is:

> **Rasterization takes geometry and turns it into a collection of screen samples that can be tested, shaded, and stored in an image.**

---

# 0. Global notation

This section establishes the vocabulary used throughout the rest of the document.

For the companion version, you can think of it as defining the rules for talking about:

- points,
- directions,
- transformations,
- shapes,
- screen coordinates,
- and perspective.

### Points and directions

A point represents a **location**.

For example:

> “The corner of the triangle is here.”

A direction represents **which way something points**, without having a particular location.

This distinction becomes important when transforming objects. Moving a point should change its location, but simply moving a direction would be incorrect.

The document uses a representation that lets the same transformation machinery handle both points and directions correctly.

### Affine transformations

An affine transformation is essentially an ordinary object transformation.

It covers operations such as:

- moving an object,
- rotating it,
- scaling it,
- and combining those operations.

These transformations preserve the basic flat structure of objects.

For example, a triangle remains a triangle after an ordinary object transformation.

### Simplex

The document introduces a general term for the kind of region produced by blending several points together.

You don't need the formal definition to understand the later sections.

For the purposes of this document:

> A simplex is a convenient way of describing shapes such as triangles using their corner points.

The important case here is the triangle.

### Perspective divide

This is one of the most important ideas in the entire document.

Before perspective is applied, the renderer is working with positions in a special four-component representation.

The perspective step converts those positions into ordinary three-dimensional coordinates.

This conversion is what makes distant objects appear smaller.

It is also the one part of the normal transformation pipeline that cannot simply be represented as an ordinary matrix transformation.

---

# 1. Transformation pipeline

This section explains how an object's coordinates eventually become coordinates on the screen.

A useful mental picture is:

**object → world → camera → projection → screen**

Each stage answers a different question.

---

## Definition 1.1 — Model and view

### Object space

Every object can have its own local coordinate system.

For example, imagine a triangle stored around the origin. Its coordinates describe the triangle relative to itself.

This is called **object space**.

### World space

The object then gets placed into the larger scene.

For example:

> “Put this triangle three meters to the left of the origin and rotate it.”

That produces the object's position in **world space**.

### Camera space

Next, the scene is described from the camera's point of view.

Instead of asking:

> “Where is this object in the world?”

we ask:

> “Where is this object relative to the camera?”

This is **camera space**.

The camera transformation essentially changes the coordinate system so that the camera becomes the reference point.

The document emphasizes that the camera transformation is particularly convenient because a properly constructed camera uses perpendicular directions. This makes reversing the camera transformation inexpensive.

---

## Definition 1.2 — Projection, clipping, and perspective

Once an object is expressed relative to the camera, the renderer has to figure out what the camera actually sees.

This is where **projection** happens.

Imagine looking through a rectangular window.

The camera can only see a certain region in front of it. Objects outside that region are irrelevant.

The projection stage converts the three-dimensional camera view into something that can eventually become a two-dimensional screen position.

### Clip space

Before performing the final perspective conversion, the renderer works in a temporary coordinate system called **clip space**.

This is an important place to perform clipping.

Why?

Because perspective conversion can behave badly for points that are behind the camera or exactly at the boundary where perspective becomes undefined.

So the safe order is:

> transform → clip → perspective conversion

rather than:

> transform → perspective conversion → try to fix the bad results afterward.

### Perspective

Perspective is what gives us the familiar visual effect:

> nearby things look large, distant things look small.

A road is a good example. Its two edges appear to get closer together as they approach the horizon.

The renderer gets this effect by dividing certain coordinates by a depth-related value.

That division is the **perspective divide**.

---

## Proposition 1.3 — Orthographic projection

Not every camera uses perspective.

An **orthographic camera** does not make distant objects smaller.

Think of an engineering drawing or blueprint.

A line remains the same apparent size regardless of how far away it is.

Because there is no perspective conversion of the same kind, orthographic projection behaves like an ordinary affine transformation.

The important takeaway is:

> Perspective is the part that makes the pipeline fundamentally different from an ordinary sequence of object transformations.

---

## Definition 1.4 — Viewport

After projection, the renderer has positions in a normalized screen-like coordinate system.

Those positions still aren't actual pixels.

The viewport transformation converts them into the dimensions of the actual image.

For example, if the image is 1920 by 1080, the viewport stage turns normalized positions into locations within that 1920-by-1080 pixel area.

It also accounts for the fact that graphics screen coordinates commonly have their vertical direction arranged differently from mathematical coordinates.

---

## Proposition 1.5 — The complete per-vertex transformation

This section puts the preceding pieces together.

For each vertex, the renderer essentially performs:

> **object coordinates → world coordinates → camera coordinates → clip coordinates → screen coordinates**

Most of the transformations can be combined ahead of time.

That means the renderer doesn't need to separately perform every transformation for every vertex.

It can prepare a combined transformation for an object and then apply it to its vertices.

There is still one special operation at the end:

> the perspective divide.

That is why the document calls it the important non-matrix part of the pipeline.

---

# Definition 1.6 — Primitives

Now the document turns from **where objects are** to **what objects actually are**.

Three examples are considered:

### Triangle

A triangle is defined by three corners.

It is the fundamental primitive for rasterization because complicated surfaces can be broken into triangles.

### Parallelogram

A parallelogram is a four-sided flat shape.

The important observation is that it can be divided into two triangles.

That means a renderer doesn't need an entirely separate rasterization algorithm for parallelograms.

### Sphere

A sphere is fundamentally different.

A triangle or parallelogram can be described directly by its corners.

A sphere is instead described by a rule saying:

> “All points at this particular distance from the center belong to the surface.”

This difference motivates the later distinction between two broad kinds of primitives.

---

# Proposition 1.7 — What transformations do to primitives

This section explains an important practical property of transformations.

### Triangles stay triangles

If you transform all three corners of a triangle, the resulting shape is still the transformed triangle.

### Parallelograms stay parallelograms

The same idea works for a parallelogram.

Its corner and direction information can be transformed directly.

### Spheres are different

A general transformation does not necessarily leave a sphere looking like a sphere.

For example, if you stretch a sphere more in one direction than another, you get an **ellipsoid**.

So:

> A sphere remains a sphere only under transformations that preserve its proportions.

This is an important distinction between polygonal primitives and curved primitives.

---

# 1.1 General framework: two structural types

This subsection introduces the main abstraction behind the rest of the document.

Instead of writing completely separate rasterization theories for triangles, rectangles, and spheres, the document divides primitives into two broad categories.

---

## Type A — Corner-defined flat shapes

These are shapes that can be described using a collection of points and the region between them.

Triangles and parallelograms belong here.

The important feature is:

> They are flat and can ultimately be represented using triangles.

This is why the renderer can use essentially the same rasterization machinery for both.

---

## Type B — Rule-defined surfaces

The second category describes surfaces using a rule that tells us whether a point belongs to the surface.

A sphere is the main example.

Instead of saying:

> “Here are the sphere's vertices.”

we can say:

> “Here is the rule describing the sphere's surface.”

This makes spheres fundamentally different from polygon meshes.

---

## Fan triangulation

The document then shows how a larger flat primitive can be divided into triangles.

For a four-sided parallelogram, draw a diagonal.

Now there are two triangles.

This is called **triangulation**.

The reason it matters is extremely practical:

> Once a shape has been converted into triangles, the renderer can use one triangle rasterizer rather than needing a separate rasterizer for every flat primitive.

---

## Checkpoint 1

The author is essentially asking:

> “Is this two-category classification enough for the renderer I'm building?”

For the examples in this document, the answer is intended to be yes:

- flat polygon-like objects can become triangles;
- curved rule-defined objects can use their own coverage and intersection logic.

The checkpoint is therefore about whether this abstraction is broad enough for the intended renderer.

---

# 1.2 Deriving each primitive's machinery from the framework

This subsection shows that the previous abstraction isn't merely organizational.

It actually lets the renderer reuse the same machinery.

---

## Proposition 1.9 — Type A primitives and the transformation pipeline

The key idea is:

> If a flat shape is divided into triangles, transforming the whole shape produces the same screen region as transforming those triangles.

So a parallelogram does not need special treatment after it has been triangulated.

The renderer can simply:

1. divide it into triangles;
2. transform their corners;
3. rasterize the triangles.

### Why this matters

It means that the triangle is effectively the basic building block for flat rasterization.

The document also points out that the way a point is represented inside a primitive changes during perspective projection, but the overall screen-space boundary still behaves properly.

That becomes particularly important when interpolating colors, textures, and depth later.

---

## Proposition 1.10 — Type B primitives

Curved surfaces require a different approach.

The renderer can transform the **description of the surface** into camera space rather than simply transforming a collection of corners.

For a sphere, there are two especially important questions.

### Coverage

Which part of the sphere is visible from the camera?

The outer boundary of the visible sphere is its **silhouette**.

Think of looking at a ball against a bright background.

The outline you see is the silhouette.

The document explains how this boundary can be determined from the geometry of the surface and the viewing direction.

### Depth

Once the renderer knows that a particular screen location hits the sphere, it needs to know:

> “How far away is the first point on the sphere along this viewing direction?”

This is almost exactly the same question a ray tracer asks.

That is why the existing ray-sphere intersection code can be reused.

---

# Corollary 1.11 — The sphere's screen outline

This section specializes the previous idea to curved surfaces such as spheres and other quadratic surfaces.

The complicated-looking derivation is ultimately answering a simple question:

> “What shape does the outline of this curved object make on the screen?”

For a sphere viewed from outside, that outline is an ellipse-like curve.

The important practical insight is not the equation itself.

It is this:

> A sphere does not necessarily need to be converted into thousands of triangles just to determine which pixels it covers.

Its coverage can instead be derived directly from its geometric description.

---

## Sphere as a special case

The document points out that a sphere is simply a particularly simple member of this more general family of curved surfaces.

That means the same machinery can handle more than just spheres.

The depth calculation is especially convenient because the ray-sphere intersection code already exists in a ray tracer.

So the renderer can reuse existing geometry code instead of inventing a completely new method.

---

# 2. The gap: from a screen-space primitive to a set of pixels

This is the central section of the document.

Everything before this point gets us from:

> “Here is a triangle in 3D.”

to:

> “Here are the triangle's corners on the screen.”

But that is **not enough**.

The screen does not draw abstract triangles.

It draws pixels.

So we now need to answer:

> Which pixels belong to the triangle?

And then:

> What information should each of those pixels receive?

The document separates this into two jobs:

### Coverage

Determine **which pixels or samples are covered**.

### Interpolation

Determine the information at those locations, such as:

- depth,
- color,
- texture coordinates,
- normals,
- and other vertex attributes.

This separation is one of the most important conceptual ideas in the document.

---

# 2.1 Coverage for a triangle: edge functions

The renderer needs a fast way to answer:

> “Is this screen location inside the triangle?”

The document's **edge functions** provide that test.

Imagine drawing each of the triangle's three edges as a line.

Each line divides the screen into two sides.

For a correctly oriented triangle, there is one side of each edge that contains the interior of the triangle.

So to determine whether a point is inside:

1. test it against the first edge;
2. test it against the second edge;
3. test it against the third edge;
4. accept it only if it is on the correct side of all three.

That is the entire idea behind the edge-function test.

### Why edge functions are useful

They are extremely cheap to evaluate.

More importantly, their values change in a predictable way as you move from one pixel to the next.

That leads directly to the optimization in the next subsection.

---

# Definition 2.3 — Barycentric weights

This is another major concept.

Barycentric coordinates are a way of describing:

> “How much of each triangle corner contributes to this location?”

Imagine the three corners of a triangle each having a color:

- corner A is red;
- corner B is green;
- corner C is blue.

A point near A should look mostly like A.

A point in the middle should contain contributions from all three corners.

Barycentric coordinates provide exactly those three contributions.

They are therefore useful for interpolating:

- colors,
- texture coordinates,
- normals,
- depth,
- and other per-vertex information.

### The important mental model

Don't think of barycentric coordinates primarily as a mathematical trick.

Think:

> **Every point inside a triangle can be described by “how much of each corner does this point belong to?”**

That is what the renderer needs.

---

# Proposition 2.4 — Barycentric coordinates and interpolation

This section establishes three important facts.

### First

The three contributions always add up to the whole point.

There isn't any missing contribution.

### Second

Inside the triangle, all three contributions are valid and non-negative.

Outside the triangle, at least one of them stops satisfying that condition.

So barycentric coordinates can double as another inside/outside test.

### Third

If each vertex has an attribute, barycentric coordinates let us smoothly extend that attribute across the triangle.

For example:

> vertex 1 has texture coordinate A  
> vertex 2 has texture coordinate B  
> vertex 3 has texture coordinate C

The renderer can determine the texture coordinate at every point between them.

This is the basic mechanism behind smooth interpolation across triangles.

---

# 2.2 Candidate pixels

It would be wasteful to test every pixel on the entire screen for every triangle.

Imagine a tiny triangle in the corner of a 4K image.

There is no reason to test millions of unrelated pixels.

Instead, the renderer first finds the triangle's **bounding box**.

That is the smallest rectangle containing the triangle.

Then it only examines pixels inside that rectangle.

For example:

> Triangle occupies roughly pixels 400–500 horizontally and 200–350 vertically.

The renderer only tests that rectangle.

### Incremental evaluation

The edge tests also have a useful property:

> Moving one pixel to the right changes the test result by a predictable amount.

Likewise, moving one pixel downward changes it predictably.

So the renderer can calculate the starting value and then mostly use simple additions while walking across the image.

This makes triangle rasterization extremely fast.

---

# 2.3 Sampling

A pixel is not actually a mathematical point.

It represents a small area of the image.

The renderer therefore needs a rule for deciding what part of that area is covered by a primitive.

### Point sampling

The simplest approach is:

> Look at one location in the center of the pixel.

If that location is inside the triangle, consider the pixel covered.

If it isn't, consider it uncovered.

This is fast and simple.

But it can produce **aliasing**.

---

### Aliasing

Aliasing is the jagged or unstable appearance that can happen when a continuous shape is represented using a discrete pixel grid.

Diagonal lines are the classic example.

A smooth diagonal line may appear as a staircase.

Small objects can also flicker or disappear as the camera moves.

---

### Multisampling

Instead of asking one question per pixel, the renderer can ask several questions at different locations within the pixel.

For example:

> “Is the triangle present at these several sub-pixel locations?”

The renderer can then estimate how much of the pixel is covered.

More samples generally provide a better approximation of the true coverage.

This is the basic idea behind **multisampling**.

The important conceptual point is:

> Sampling is separate from the triangle-inside test.

The edge functions tell us whether a particular sample is inside.

The sampling strategy tells us **where we choose to perform those tests**.

---

# 2.4 Fill rule

There is a subtle problem when two triangles share an edge.

Suppose two triangles meet perfectly along one boundary.

If both triangles include that boundary, the renderer could draw it twice.

If neither includes it, there could be a tiny gap.

Neither behavior is desirable.

A **fill rule** solves this by deciding which triangle owns boundary samples.

The document discusses a common approach based on consistently treating particular edge orientations as inclusive and others as exclusive.

The important idea is:

> **Shared edges need a deterministic ownership rule.**

That guarantees that two neighboring triangles meet cleanly.

This is particularly important for meshes, where thousands of triangles may share edges.

---

# 2.5 Parallelogram and sphere via Section 1.2

Now the earlier abstraction pays off.

### Parallelogram

A parallelogram is split into two triangles.

Therefore:

> The normal triangle rasterizer handles it.

There is no need for a special parallelogram rasterizer.

### Sphere

The sphere follows the other branch of the framework.

Its coverage can be determined from its screen-space silhouette, while its depth can be determined using the same kind of intersection calculation used by a ray tracer.

So the rasterizer can potentially support both:

- ordinary triangle meshes;
- analytic curved objects.

And they can ultimately feed the same depth buffer.

---

# Checkpoint 2

This is the second design checkpoint.

The document is asking:

> “Have we now actually solved the missing part between screen-space geometry and pixels?”

The intended answer is that we have the basic pieces:

- determine candidate pixels;
- determine which samples are covered;
- determine where each covered sample lies inside the primitive;
- interpolate its attributes.

That is the core of rasterization.

---

# 3. Depth buffer mechanics

Once we know which pixels a primitive covers, another problem appears.

Suppose two triangles cover the same pixel.

Which one should appear?

The answer is:

> The one that is closer to the camera.

A **depth buffer** keeps track of that information.

You can think of it as an invisible second image.

The color buffer stores:

> “What color should this pixel be?”

The depth buffer stores:

> “How close is the visible object at this pixel?”

---

# 3.1 The buffer

At the beginning of a frame, the depth buffer is filled with a value representing:

> “Nothing is currently visible here.”

Then primitives are rasterized.

Whenever a primitive covers a sample, the renderer compares its depth with the depth already stored there.

If the new primitive is closer:

> replace the old depth and replace the pixel's color.

If it is farther away:

> ignore it.

This is the **depth test**.

### Why this is powerful

Objects can be processed in almost any order.

You don't have to carefully draw all the far-away objects first.

The depth buffer automatically keeps the closest result.

---

# 3.2 Naive interpolation is wrong

This is one of the technically important warnings in the document.

It is tempting to take the depth values at the three triangle corners and simply blend them across the triangle using ordinary screen-space interpolation.

That works when the transformation from the triangle to the screen is effectively flat.

But perspective projection is not flat in that way.

Perspective changes how positions are distributed across the screen.

Therefore:

> Directly blending the post-perspective depth values can produce incorrect depth.

The same issue affects other attributes, such as texture coordinates.

This is why the next section is necessary.

---

# 3.3 Perspective-correct interpolation

This section explains how the renderer fixes the previous problem.

The basic idea is:

> Don't interpolate the final attribute directly.

Instead, temporarily adjust the attribute using the depth-related information from the perspective transformation.

Then interpolate those adjusted values.

Finally, undo the adjustment.

The result is **perspective-correct interpolation**.

### Why this matters visually

Consider a textured triangle.

Without perspective correction, the texture can appear to stretch or distort incorrectly.

With perspective correction, the texture behaves as though it were attached to the actual three-dimensional surface.

The same principle applies to depth.

### A useful mental picture

Imagine a checkerboard lying on the floor.

The squares near you should appear large.

The squares farther away should become progressively smaller.

Naive screen-space interpolation does not naturally produce that behavior.

Perspective-correct interpolation does.

---

## Perspective-correct depth

Depth is just another value that needs to be interpolated correctly.

The document points out that the renderer may store a normalized depth value rather than the camera-space distance itself.

This is important later when discussing depth precision.

The key idea is:

> **The depth buffer only works correctly if the renderer gives it the right depth value for each sample.**

---

# 3.4 Order-independence

This section explains an important consequence of the depth buffer.

Suppose three opaque objects cover the same pixel.

The renderer can encounter them in this order:

> A → B → C

or:

> C → A → B

or:

> B → C → A

The final visible object is still the closest one.

That's because every new depth simply competes with the depth already stored.

The buffer is effectively keeping:

> “the closest result seen so far.”

### Important exception

This does **not** automatically work for transparency.

Transparent objects depend on blending with what is behind them.

Therefore, transparency generally introduces ordering requirements that opaque depth testing does not have.

---

# 3.5 Contrast with ray tracing

This section connects rasterization directly to the ray tracer.

The two rendering approaches are solving a surprisingly similar visibility problem.

### Ray tracing

A ray tracer asks:

> “For this pixel, which object does the ray hit first?”

It starts with a pixel and searches through objects.

Conceptually:

> pixel → possible objects → closest hit

### Rasterization

Rasterization does almost the opposite.

It starts with an object and asks:

> “Which pixels does this object cover?”

Then it submits those pixels to the depth buffer.

Conceptually:

> object → covered pixels → depth test

### The important insight

These are essentially two different ways of organizing the same visibility problem.

One is:

> **pixel first, objects second**

The other is:

> **object first, pixels second**

The depth buffer works because choosing the closest depth is independent of the order in which opaque objects are processed.

---

## Why rasterization doesn't naturally give you the second-closest object

The ordinary depth buffer only remembers one thing:

> the closest object found so far.

Once a farther object loses the depth test, its information is discarded.

A ray tracer, on the other hand, can explicitly ask for:

> first hit, second hit, third hit, etc.

This is one reason ray tracing naturally extends into effects involving secondary rays.

---

# 4. Full pipeline: order of operations

This section turns the previous concepts into an actual renderer workflow.

Here is the plain-English version.

---

## Step 1 — Start a new frame

Clear:

- the color image;
- the depth buffer.

The depth buffer begins in a state meaning:

> “Nothing has been drawn here yet.”

---

## Step 2 — Prepare each object

For each object, combine the transformations needed to take it from its local coordinate system all the way toward the screen.

This can be prepared once for the object instead of rebuilding the transformation chain for every vertex.

---

## Step 3 — Process each primitive

For every triangle:

### A. Transform its vertices

Move the vertices through the rendering pipeline until their screen positions are known.

Keep the depth-related information needed later for perspective-correct interpolation.

### B. Clip

Remove portions that are outside the camera's usable viewing region.

This must happen before the perspective divide when necessary.

The reason is simple:

> Perspective conversion cannot safely handle positions behind the camera or at its problematic boundary.

### C. Backface culling

Optionally discard triangles facing away from the camera.

If a closed object is opaque, the back side often doesn't need to be rendered.

This can save substantial work.

### D. Find candidate pixels

Find the triangle's bounding rectangle.

Then test the relevant samples using the edge tests.

Apply the chosen fill rule.

### E. Interpolate attributes

For every covered sample, determine its:

- depth;
- color;
- texture coordinates;
- normals;
- and other attributes.

Use perspective-correct interpolation where required.

### F. Perform the depth test

Compare the new depth against the depth already stored.

If the new primitive is closer:

> update the depth and color.

Otherwise:

> do nothing.

---

## Step 4 — Finish the frame

Once all primitives have been processed, the color buffer contains the rendered image.

That is the complete rasterization pipeline.

---

## Why the order of objects doesn't matter

For opaque objects, the depth test guarantees that the closest object eventually wins.

Therefore, object order can generally be changed without changing the final image.

Again, transparency is an important exception.

---

# 5. Edge cases, derived

This section deals with situations where the basic pipeline can produce surprising results or numerical problems.

These are not really separate rendering concepts.

They are the places where a practical implementation needs extra care.

---

# Proposition 5.1 — Depth precision

Depth buffers do not distribute their precision evenly across the entire visible range.

They are much more sensitive to differences between nearby objects close to the camera than to differences between distant objects.

This has an important consequence.

Suppose your camera can see from:

> very close → extremely far away.

A large portion of the available depth precision can be concentrated near the near clipping plane.

That means two surfaces far away from the camera can end up with very similar stored depth values even when they are separated by a meaningful world-space distance.

This can cause **z-fighting**.

---

## Z-fighting

Imagine two nearly overlapping surfaces.

The depth buffer has trouble deciding which one is slightly closer because their stored depth values become indistinguishable.

The visible result can flicker or appear as patterns fighting between the two surfaces.

This is especially common with:

- roads and decals;
- nearly overlapping polygons;
- coplanar surfaces;
- large scenes with extreme depth ranges.

---

## Near and far clipping planes

One of the practical conclusions is:

> The choice of the near clipping plane matters enormously.

Moving the near plane farther from the camera can improve useful depth precision.

Changing the far plane is generally less helpful than people initially expect.

---

## Reverse-Z

The document also mentions **reverse-Z** as a technique for making better use of floating-point depth precision.

The basic idea is to arrange the depth representation so that its precision is better matched to how floating-point numbers are distributed.

You don't need the mathematical details to understand the motivation:

> **Reverse-Z is a way of reorganizing depth values so that the depth buffer makes better use of the precision available to it.**

---

# Proposition 5.2 — Degenerate triangles

A triangle is **degenerate** when its three corners effectively lie along the same line.

Visually, it has collapsed into something with no area.

Such a triangle cannot cover any meaningful region of the screen.

This causes a practical problem because the barycentric interpolation system expects a real triangle with area.

If the triangle has collapsed, the interpolation calculation no longer has a meaningful answer.

### Why this can happen unexpectedly

A triangle that is perfectly valid in 3D can become degenerate on the screen.

For example:

> Look directly along the edge of a triangle.

The triangle can appear to collapse into a line.

Therefore, degeneracy should be checked **after transformation into screen space**, not merely in object space.

---

## Parallelogram degeneracy

The same idea applies to a parallelogram.

If its two defining directions collapse onto the same direction, the parallelogram has effectively collapsed.

Again, it cannot produce a proper two-dimensional region for rasterization.

---

# Remark 5.3 — Points behind the camera

Perspective conversion is not valid for points on the wrong side of the camera.

Therefore:

> clip first, perspective divide second.

This is not merely an optimization.

It is required to prevent invalid or wildly incorrect screen coordinates.

---

# Remark 5.4 — Very different scales

Large differences in scale can cause precision problems.

For example, imagine one object is enormous while another is extremely tiny.

The renderer uses finite-precision numbers, so it cannot represent every possible value perfectly.

Operations such as:

- edge tests;
- depth calculations;
- coordinate subtraction

can lose precision when values differ greatly in scale.

Possible solutions include:

- using camera-relative coordinates;
- splitting depth ranges;
- organizing the scene into multiple passes.

The general lesson is:

> Numerical precision is part of graphics engineering, not just a theoretical concern.

---

# Remark 5.5 — Shared-edge numerical drift

Two neighboring triangles may share exactly the same geometric edge.

In theory, both triangles should agree perfectly about which pixels belong to that edge.

In real floating-point arithmetic, tiny rounding differences can cause the two calculations to disagree in their last few bits.

This can produce:

- tiny gaps;
- overlaps;
- inconsistent edge ownership.

A fixed sub-pixel representation can help ensure that shared vertices and edges are represented identically.

The deeper lesson is:

> A logically correct algorithm can still need numerical safeguards when implemented with floating-point arithmetic.

---

# Remark 5.6 — Spheres

The document distinguishes two ways of rendering spheres.

### Tessellated sphere

Turn the sphere into many triangles.

Advantages:

- uses the ordinary triangle pipeline;
- easy to integrate;
- works naturally with mesh rendering.

Disadvantage:

- the sphere is only an approximation;
- more triangles are needed for a smoother silhouette.

### Implicit sphere

Keep the sphere as an actual sphere.

Determine:

- which screen samples it covers;
- how far away the first intersection is.

Advantages:

- exact sphere geometry;
- existing ray-sphere intersection code can be reused.

The important architectural point is:

> Both approaches can ultimately write their results into the same depth buffer.

The depth buffer doesn't care where the depth value came from.

---

# 6. Bridging to the ray tracer

This is the final conceptual connection.

The rasterizer and ray tracer do not have to be completely separate systems.

They can share a substantial amount of infrastructure.

---

## Shared front end

Both renderers need to understand:

- objects;
- transformations;
- cameras;
- world space;
- camera space;
- projection.

Therefore, the transformation and scene setup code can be shared.

---

## Where the renderers diverge

The major difference appears when deciding visibility.

### Ray tracer

A ray tracer asks:

> “What does this particular ray hit first?”

It can cast rays in arbitrary directions.

That makes secondary effects natural:

- reflections;
- refractions;
- shadow rays;
- indirect rays.

### Rasterizer

A rasterizer starts with the primitives.

It asks:

> “Which screen samples does this primitive affect?”

Then the depth buffer determines which primitive wins.

So the rasterizer uses persistent per-pixel state, while the ray tracer can determine visibility independently for each ray.

---

# Triangles and parallelograms

For triangles and parallelograms, the two systems can share the same geometric representation.

The rasterizer:

> transforms → covers pixels → interpolates → depth-tests.

The ray tracer:

> creates a ray → tests intersection → keeps the closest hit.

The underlying geometry is the same.

Only the way visibility is requested is different.

---

# Spheres

Spheres provide an especially interesting bridge.

A rasterizer can:

### Option 1 — Tessellate

Turn the sphere into triangles and send it through the ordinary rasterization pipeline.

### Option 2 — Keep it analytic

Determine its screen-space coverage directly, then use the existing ray-sphere intersection routine to determine depth.

The second approach is particularly useful because it allows the ray tracer's existing intersection code to be reused.

Instead of asking:

> “Does this sphere intersect every ray in the image?”

the rasterizer asks:

> “Does this sphere cover this particular pixel?”

and, if it does:

> “Where is the first sphere intersection for this pixel?”

That is perhaps the clearest connection between the two rendering approaches.

---

# The big picture

The entire document can be reduced to one pipeline:

**3D primitive**

↓  

**transform into camera and screen space**

↓

**determine its screen-space footprint**

↓

**find the covered pixels or samples**

↓

**interpolate the primitive's attributes**

↓

**calculate depth**

↓

**compare against the depth buffer**

↓

**keep the closest result**

↓

**write the final color**

The central conceptual gap is therefore not:

> “How do I draw a triangle?”

It is:

> **“How do I turn a geometric object into a reliable set of screen samples, each carrying the correct depth and attributes?”**

Once that gap is understood, the rest of a basic rasterizer becomes much easier to organize.

And the connection to the ray tracer becomes clear:

> **Ray tracing and rasterization use different ways of organizing the visibility problem, but they can share the same scene representation, transformations, geometry, and even intersection routines.**