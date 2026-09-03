# From Primitives to a Depth Buffer: Closing the Rasterization Gap

*A note on calibration:* since this is a static document, I'm stating assumptions rather than asking. I'm assuming full comfort with affine/projective algebra and homogeneous coordinates; ray tracing's visibility model ($t^\ast = \min\{t>0 : r(t)\in \text{primitive}\}$) as your mental anchor; "arbitrary primitives" meaning the sphere/parallelogram/triangle cases should be presented as instances of a general pattern; and that your gap is specifically **primitive → coverage/samples → interpolated attributes**, with the depth buffer as the consumer of that step's output. Below I use **Definition / Proposition / Proof / Remark** blocks throughout and keep prose to a minimum — flag anywhere a Proof is too terse to reconstruct.

---

## 0. Global notation

- $\iota:\mathbb R^3\to\mathbb R^4$, $\iota(p)=(p,1)^\top=:\tilde p$ for points; $\iota_0(v)=(v,0)^\top$ for vectors/directions. The $w$-coordinate ($1$ vs.\ $0$) is what makes a single matrix act correctly on both.
- $\mathrm{Aff}(3) := \{x\mapsto Ax+b : A\in\mathbb R^{3\times3},\,b\in\mathbb R^3\}$, represented as $M=\begin{pmatrix}A&b\\0^\top&1\end{pmatrix}\in\mathbb R^{4\times4}$, acting by $\tilde p\mapsto M\tilde p$. The map $\mathrm{Aff}(3)\to\{M : \text{bottom row}=(0,0,0,1)\}$ is a group isomorphism onto its image under composition, i.e. $M_{g_2\circ g_1} = M_{g_2}M_{g_1}$.
- $\Delta^{n-1} := \{\theta\in\mathbb R^n : \theta_i\ge0\;\forall i,\ \textstyle\sum_i\theta_i=1\}$, the standard simplex.
- $\Pi:\{(x,y,z,w)\in\mathbb R^4 : w\ne0\}\to\mathbb R^3$, $\Pi(x,y,z,w) := (x/w,\,y/w,\,z/w)$: the perspective divide.

---

## 1. Transformation pipeline

**Definition 1.1 (Model, view).** For object $k$, $M_{\text{obj}\to\text{world}}^{(k)}\in\mathrm{Aff}(3)$ encodes its local frame. For a camera at $e\in\mathbb R^3$ with orthonormal frame $(u,v,w)$,
$$
M_{\text{world}\to\text{cam}} := \begin{pmatrix}u^\top & -u^\top e\\ v^\top & -v^\top e\\ w^\top & -w^\top e\\ 0^\top & 1\end{pmatrix}\in\mathrm{Aff}(3).
$$
*Remark.* Since $(u,v,w)$ orthonormal $\Rightarrow A^{-1}=A^\top$ for $A=(u\,v\,w)^\top$, this is the exact, cheap inverse of the map placing the camera frame into world space. $M_{\text{world}\to\text{cam}}\,M_{\text{obj}\to\text{world}}^{(k)}\in\mathrm{Aff}(3)$ by closure of $\mathrm{Aff}(3)$ under composition.

**Definition 1.2 (Projection, clip $w$, perspective divide).** For near/far planes $n,f$ and frustum extents,
$$
M_{\text{proj}} := \begin{pmatrix}\frac{2n}{r-l}&0&\frac{r+l}{r-l}&0\\0&\frac{2n}{t-b}&\frac{t+b}{t-b}&0\\0&0&-\frac{f+n}{f-n}&-\frac{2fn}{f-n}\\0&0&-1&0\end{pmatrix}\in\mathbb R^{4\times4}.
$$
Its bottom row $\ne(0,0,0,1)$, so $M_{\text{proj}}\notin\mathrm{Aff}(3)$. Write $\tilde c := M_{\text{proj}}\tilde p_{\text{cam}}=(x_c,y_c,z_c,w_c)^\top$; for this matrix, $w_c=-z_{\text{cam}}$. Define $x_n:=\Pi(\tilde c)$ (NDC), well-defined for $w_c\ne0$.

**Proposition 1.3 (Orthographic $\Rightarrow$ affine).** If $M_{\text{proj}}$'s bottom row is $(0,0,0,1)$, then $w_c\equiv1$ and $\Pi\circ M_{\text{proj}}\big|_{\{w=1\}}$ coincides with the affine map given by $M_{\text{proj}}$'s top $3\times4$ block. *Proof.* Immediate: $w_c=1\Rightarrow\Pi(\tilde c)=(x_c,y_c,z_c)$, the direct output of a linear map applied to $\tilde p_{\text{cam}}$ with fixed $w=1$, i.e. an affine map of $p_{\text{cam}}$. $\blacksquare$

**Definition 1.4 (Viewport).** $M_{\text{viewport}}\in\mathrm{Aff}(3)$, $\begin{pmatrix}W/2&0&0&W/2\\0&-H/2&0&H/2\\0&0&1/2&1/2\\0&0&0&1\end{pmatrix}$, applied to $\iota(x_n,y_n,z_n)$, i.e. strictly after $\Pi$ — the reason it is permitted to be affine despite following the pipeline's one nonlinear step.

**Proposition 1.5 (Full per-vertex map).**
$$
w_c\cdot\tilde p_{\text{screen}} = M_{\text{viewport}}\cdot\iota\Big(\Pi\big(M_{\text{proj}}\,M_{\text{world}\to\text{cam}}\,M_{\text{obj}\to\text{world}}^{(k)}\,\tilde p_{\text{obj}}\big)\Big),
$$
with $M := M_{\text{proj}}\,M_{\text{world}\to\text{cam}}\,M_{\text{obj}\to\text{world}}^{(k)}$ precomputable as one $4\times4$ matrix per object per frame; $\Pi$ is the unique non-precomputable, non-matrix step.

**Definition 1.6 (Primitives).**
$$
T(p_0,p_1,p_2) := \Big\{\textstyle\sum_i\theta_ip_i : \theta\in\Delta^2\Big\}, \qquad
Q(p_0,e_1,e_2) := \{p_0+se_1+te_2 : s,t\in[0,1]\}, \qquad
S(c,r) := \{x\in\mathbb R^3 : \|x-c\|^2=r^2\}.
$$

**Proposition 1.7.** For $g:x\mapsto Ax+b\in\mathrm{Aff}(3)$: $g(T(p_0,p_1,p_2))=T(gp_0,gp_1,gp_2)$; $g(Q(p_0,e_1,e_2))=Q(gp_0,\,Ae_1,\,Ae_2)$ (vectors transform by $A$ alone, since $\iota_0(e_i)$ has $w=0$); and $g(S(c,r)) = \{x : (x-c')^\top Q(x-c')=1\}$ for $c'=g(c)$, $Q=(AA^\top)^{-1}/r^2$ — an ellipsoid, a sphere only when $A$ is a similarity. *Proof.* First two: direct substitution using $g(\sum\theta_ip_i)=\sum\theta_ig(p_i)$ for $\sum\theta_i=1$ (affine maps preserve affine/convex combinations) and $g(p_0+se_1+te_2) = g(p_0)+sAe_1+tAe_2$. Third: substitute $x=Ay+b\Leftrightarrow y=A^{-1}(x-b)$ into $\|y-c\|^2=r^2$. $\blacksquare$

---

### 1.1 General framework: two structural types

**Definition 1.8 (Type A, Type B).**
$$
\textbf{Type A: } \mathcal P_A(p_0,\dots,p_{n-1}) := \mathrm{conv}\{p_i\}_{i=0}^{n-1}, \quad \exists\,x_0,u,v\in\mathbb R^3,\ (a_i,b_i)\in\mathbb R^2 \text{ s.t. } p_i=x_0+a_iu+b_iv\ \forall i,
$$
$$
\textbf{Type B: } \mathcal P_B(F) := \{x\in\mathbb R^3 : F(x)=0\}, \quad F:\mathbb R^3\to\mathbb R \text{ not affine.}
$$
$T,Q$ are Type A ($n=3,4$, coplanar by hypothesis); $S(c,r)=\mathcal P_B(F)$, $F(x)=\|x-c\|^2-r^2$. Fan-triangulation $T_k:=T(p_0,p_k,p_{k+1})$, $k=1,\dots,n-2$, decomposes any Type A primitive into $n-2$ triangles, e.g. $Q(p_0,e_1,e_2)$ into $T_1=T(p_0,p_0{+}e_1,p_0{+}e_1{+}e_2)$, $T_2=T(p_0,p_0{+}e_1{+}e_2,p_0{+}e_2)$.

**Checkpoint 1.** If the Type A/B split is the right abstraction, §2–§6 refer back to it by name rather than re-deriving triangles/parallelograms/spheres separately. Flag now if a third type (e.g. parametric surfaces) is needed, or if this split should be finer.

### 1.2 Deriving each primitive's machinery from the framework

**Proposition 1.9 (Type A: fan triangulation commutes with the pipeline).** Let $\theta\in\Delta^{n-1}$, $x(\theta):=\sum_i\theta_ip_i$, so $x(\Delta^{n-1})=\mathcal P_A(p_0,\dots,p_{n-1})$. Let $\tilde c_i := M_{\text{proj}}\tilde p_i =: (x_{c,i},y_{c,i},z_{c,i},w_{c,i})^\top$, and suppose $w_{c,i}>0\;\forall i$ (guaranteed by clipping, §4b). Then:
$$
\text{(a)}\quad x_n(\Delta^{n-1}) = \mathrm{conv}\{(x_{n,i},y_{n,i})\}_{i=0}^{n-1}, \qquad x_{n,i}:=x_{c,i}/w_{c,i};
$$
$$
\text{(b)}\quad \theta_k=0 \;\Rightarrow\; \rho_k(\theta)=0, \quad \text{where } \rho_i(\theta):=\theta_iw_{c,i}\Big/\textstyle\sum_j\theta_jw_{c,j}.
$$
I.e. the pipeline maps the hull to the hull of the images and edges to edges — fan triangles remain non-degenerate, non-self-intersecting triangles after the full transform.

*Proof.* $M_{\text{proj}}$ linear and $\sum_i\theta_i=1$ give $M_{\text{proj}}\tilde x(\theta)=\sum_i\theta_i\tilde c_i$, hence $w_c(\theta)=\sum_i\theta_iw_{c,i}$ and $x_n(\theta) = \big(\sum_i\theta_ix_{c,i}\big)\big/\big(\sum_j\theta_jw_{c,j}\big)$. Define $\rho_i(\theta):=\theta_iw_{c,i}/\sum_j\theta_jw_{c,j}$; since $\theta_i\ge0,\,w_{c,i}>0$, $\rho_i(\theta)\ge0$ and $\sum_i\rho_i(\theta)=1$, so $\rho(\theta)\in\Delta^{n-1}$, and by direct substitution
$$
x_n(\theta) = \sum_i\rho_i(\theta)\,x_{n,i}.
\tag{$\ast$}
$$
**Containment $\subseteq$:** ($\ast$) exhibits $x_n(\theta)$ as an explicit convex combination of $\{x_{n,i}\}$, so $x_n(\theta)\in\mathrm{conv}\{x_{n,i}\}\;\forall\theta\in\Delta^{n-1}$.
**Containment $\supseteq$:** $\theta\mapsto\rho(\theta)$ is continuous on $\Delta^{n-1}$ (ratio of linear functions, denominator $>0$), satisfies $\rho(e_i)=e_i$ (direct substitution), and has continuous inverse $\theta_i = (\rho_i/w_{c,i})/\sum_j(\rho_j/w_{c,j})$ (verify: substitute back into $\rho_i(\theta)$, telescopes to identity). A continuous, vertex-fixing bijection $\Delta^{n-1}\to\Delta^{n-1}$ is surjective — given $\rho^\ast\in\Delta^{n-1}$, the inverse formula produces $\theta^\ast$ with $\rho(\theta^\ast)=\rho^\ast$ — so every $\sum_i\rho^\ast_i x_{n,i}$ is attained, giving $\mathrm{conv}\{x_{n,i}\}\subseteq x_n(\Delta^{n-1})$. Combined: (a).
**(b):** the formula $\rho_k(\theta)=\theta_kw_{c,k}/\sum_j\theta_jw_{c,j}$ vanishes whenever $\theta_k=0$, regardless of the other $\theta_j$; direct substitution. $\blacksquare$

*Corollary.* $T_k$'s image under the full pipeline is $T(\tilde p_{\text{screen},0},\tilde p_{\text{screen},k},\tilde p_{\text{screen},k+1})$ for every $k$; §2–§3 apply to each verbatim. $Q$ ($n=4$) and $T$ ($n=3$, one trivial fan piece) require no separate proof.

**Proposition 1.10 (Type B: pullback, silhouette, and ray intersection).** Let $F:\mathbb R^3\to\mathbb R$, $\Sigma=\mathcal P_B(F)$ in object space, and let $x=Ay+b$ be the (invertible) composite object$\to$camera map. Then:
$$
\text{(i)}\quad \Sigma' := \{x : F(A^{-1}(x-b))=0\} \quad \text{(pullback, not pushforward — no vertices to push).}
$$
$$
\text{(ii, coverage)}\quad \partial_{\text{sil}}\Sigma' = \{x\in\Sigma' : \nabla F(A^{-1}(x-b))\cdot\hat d(x) = 0\}, \quad \hat d(x):=\text{view direction to }x.
$$
$$
\text{(iii, depth)}\quad \forall\,(x,y)\in\mathbb R^2:\ t^\ast(x,y) := \min\{t>0 : F(A^{-1}(t\,d - b))=0\},\quad d=(x,y,-1)^\top.
$$

*Proof.* (i): $x\in\Sigma' \Leftrightarrow A^{-1}(x-b)\in\Sigma \Leftrightarrow F(A^{-1}(x-b))=0$, by definition of the pushed-forward point set under the coordinate change $y=A^{-1}(x-b)$. (ii): the silhouette is exactly the zero-set locus where the ray direction is tangent to (lies in the tangent plane of) $\Sigma'$, i.e. orthogonal to the gradient. (iii): pinhole ray through pixel $(x,y)$ in camera space is $r(t)=td$; pulling back to object space via $y(t)=A^{-1}(r(t)-b)$ and imposing $F(y(t))=0$ gives the intersection condition; $t^\ast$ selects the first (smallest positive) root. $\blacksquare$

**Corollary 1.11 (Explicit conic for quadric $F$).** Let $\Sigma'=\{q : q^\top Qq+2b^\top q+\kappa=0\}$ ($Q=Q^\top\in\mathbb R^{3\times3}$, camera space, pinhole image plane $z=-1$). Then the silhouette, expressed in image-plane coordinates $(x,y)$, is the conic $Ax^2+Bxy+Cy^2+Dx+Ey+F=0$ with
$$
A=b_1^2-\kappa Q_{11},\ \ B=2(b_1b_2-\kappa Q_{12}),\ \ C=b_2^2-\kappa Q_{22},\ \ D=2(\kappa Q_{13}-b_1b_3),\ \ E=2(\kappa Q_{23}-b_2b_3),\ \ F=b_3^2-\kappa Q_{33}.
$$
*Proof.* Set $d=(x,y,-1)^\top$; substituting $r(t)=td$ into $q^\top Qq+2b^\top q+\kappa=0$ gives $\alpha(d)t^2+\beta(d)t+\kappa=0$ with $\alpha(d)=d^\top Qd$, $\beta(d)=2b^\top d$. Tangency $\Leftrightarrow$ discriminant $=0$:
$$
\beta(d)^2-4\alpha(d)\kappa=0 \;\Longleftrightarrow\; (b^\top d)^2=\kappa\,(d^\top Qd).
$$
Expand $(b^\top d)^2 = b_1^2x^2+b_2^2y^2+b_3^2+2b_1b_2xy-2b_1b_3x-2b_2b_3y$ and $d^\top Qd = Q_{11}x^2+Q_{22}y^2+Q_{33}+2Q_{12}xy-2Q_{13}x-2Q_{23}y$; collect powers of $(x,y)$ in $(b^\top d)^2-\kappa(d^\top Qd)=0$ to read off $A$–$F$ above. $\blacksquare$

*Remark (sphere as $Q=I$).* $S(c,r)$, camera-space center $c'$: $Q=I,\,b=-c',\,\kappa=\|c'\|^2-r^2$ (matching $(x-c')^\top(x-c')=r^2$ expanded) recovers every sphere-specific statement above as the $Q=I$ instance; conic classification of $(A,B,C)$ then confirms an ellipse for any sphere with the camera outside it. Depth from (iii) with quadratic $F$ specializes to the quadratic formula on $\alpha(d)t^2+\beta(d)t+\kappa=0$ — verbatim the ray–sphere intersection already present in a ray tracer. Coverage (ii) is genuinely new relative to a pure ray tracer (which never needs "which pixels" as a question); depth (iii) is not new code, only a change in which rays get cast: one per covered pixel of a rasterization pass rather than one per pixel of a full image trace.

---

## 2. The gap: from a screen-space primitive to a set of pixels

**Setup.** Post-Proposition 1.5 and $\Pi$, each Type A primitive has vertices $(x_{s,i},y_{s,i})\in\mathbb R^2$ in screen space plus retained $w_{c,i}$ (needed by §3). Rasterization is the map
$$
\text{primitive} \;\longmapsto\; \{(i,j,z_{ij},\phi_{ij}) : (i,j)\ \text{a covered pixel}\},
$$
factoring into **coverage** (which $(i,j)$) and **interpolation** (which $z_{ij},\phi_{ij}$).

### 2.1 Coverage for a triangle: edge functions

**Definition 2.1.** For $p_i,p_j\in\mathbb R^2$: $E_{ij}(x,y) := (x-x_i)(y_j-y_i)-(y-y_i)(x_j-x_i)$.

**Proposition 2.2.** $E_{ij}$ is affine in $(x,y)$; $E_{ij}(x,y) = 2\,\mathrm{Area}_{\pm}(p_i,p_j,(x,y))$ (signed); with $(p_0,p_1,p_2)$ counterclockwise,
$$
(x,y)\in T(p_0,p_1,p_2) \;\Longleftrightarrow\; E_{01}(x,y)\ge0 \,\wedge\, E_{12}(x,y)\ge0 \,\wedge\, E_{20}(x,y)\ge0.
$$
*Proof.* $E_{ij}$ is the $z$-component of $(p_j-p_i)\times((x,y)-p_i)$ in the plane; its sign is the half-plane test for the line through $p_i,p_j$; the conjunction of the three half-plane memberships (consistent orientation) is exactly hull membership for a triangle. $\blacksquare$

**Definition 2.3 (Barycentric weights).** $2\mathcal A := E_{12}(p_0)$; $\lambda_0(x,y):=E_{12}(x,y)/2\mathcal A$, $\lambda_1:=E_{20}/2\mathcal A$, $\lambda_2:=E_{01}/2\mathcal A$.

**Proposition 2.4.** $\sum_i\lambda_i\equiv1$; $\lambda_i(x,y)\ge0\;\forall i \Leftrightarrow (x,y)\in T(p_0,p_1,p_2)$; and $\forall$ attribute $\phi$ with vertex values $\phi_i$, $\phi(x,y):=\sum_i\lambda_i(x,y)\phi_i$ is the unique affine extension of $\phi_i$ to the triangle. *Proof.* Sum identity: each $E_{ij}$ affine, direct expansion of $E_{01}+E_{12}+E_{20}$ shows the linear-in-$(x,y)$ terms cancel, leaving the constant $2\mathcal A$. Sign statement: Proposition 2.2 restated after dividing by $2\mathcal A>0$ (ccw case). Uniqueness: an affine function on $\mathbb R^2$ is determined by 3 non-collinear values, and $\sum\lambda_i\phi_i$ matches $\phi_i$ at each $p_i$ ($\lambda(p_i)=e_i$) while being affine (linear combination of affine $\lambda_i$). $\blacksquare$

### 2.2 Candidate pixels

$\mathrm{bbox}(p_0,p_1,p_2)\cap[0,W]\times[0,H]$ restricts the search; $E_{ij}$ affine $\Rightarrow \Delta E_{ij}/\Delta x = (y_j-y_i)$, $\Delta E_{ij}/\Delta y=-(x_j-x_i)$, constants — incremental evaluation by addition, no per-pixel multiplication.

### 2.3 Sampling

Pixel $(i,j) \leftrightarrow$ region $[i,i{+}1)\times[j,j{+}1)$; true coverage is an area fraction. Point sampling: evaluate at $(i+\tfrac12,j+\tfrac12)$ only (source of aliasing). Multisampling: evaluate at $k$ sub-pixel points, coverage $:= \#\{\text{inside}\}/k$. Orthogonal design axis to §2.1's evaluation rule.

### 2.4 Fill rule

**Proposition 2.5.** Let $T,T'$ share edge $e=(p_i,p_j)$ with opposite orientation in their respective windings. A rule assigning $e$ to exactly one of $\{\ge,>\}$ per triangle, consistently as a function of $e$'s geometric type (top / left edge, fixed screen orientation) rather than which triangle is being tested, partitions the shared edge's points between $T,T'$ with no overlap and no gap. *Proof sketch.* The rule is a function of the edge alone (top-left classification is orientation-independent up to the sign flip that swapping $i,j$ already encodes), so both triangles agree on which one owns $e$; $\ge$ vs.\ $>$ on the boundary itself is thus assigned to exactly one side. $\blacksquare$

### 2.5 Parallelogram and sphere via §1.2

By Proposition 1.9: $Q(p_0,e_1,e_2)\to T_1\cup T_2$, apply §2.1–2.4 to each, no new machinery. By Corollary 1.11 + Proposition 1.10(iii): $S(c,r)$ (implicit route) gets coverage from the derived conic, depth per covered pixel from $t^\ast$ — ray–sphere intersection, invoked once per covered pixel rather than once per image pixel.

**Checkpoint 2.** Core of the stated gap. Before §3 (depth buffer) and §3.5 (ray-tracing contrast) — does Definitions 2.1–2.4 + Corollary 1.11 fully close it, or is more needed on §2.3's sampling axis?

---

## 3. Depth buffer mechanics

### 3.1 The buffer

**Definition 3.1.** $D:\{0,\dots,W{-}1\}\times\{0,\dots,H{-}1\}\to\mathbb R$, $D(i,j)\leftarrow z_{\text{far}}$ initially. Update rule, $\forall$ covered sample $(i,j)$ with primitive depth $z$:
$$
z < D(i,j) \;\Rightarrow\; D(i,j)\leftarrow z,\ \ \mathrm{Color}(i,j)\leftarrow\mathrm{shade}(\phi_{ij}).
$$

### 3.2 Naive interpolation is wrong

**Remark.** Setting $z(x,y):=\sum_i\lambda_i(x,y)z_i$ with screen-space $\lambda_i$ (Definition 2.3) and post-divide $z_i$ presumes the object$\to$screen map is affine on the triangle; by Definition 1.2, camera$\to$NDC is affine composed with the nonlinear $\Pi$, so this presumption fails whenever $w_{c,i}$ are not all equal.

### 3.3 Perspective-correct interpolation

**Proposition 3.2.** Let $q_0,q_1,q_2\in\mathbb R^3$ be a triangle's camera-space vertices, $\phi_i$ any attribute's vertex values, $w_{c,i}:=-z_{{\text{cam}},i}$. Then $\forall(x,y)$ in the triangle's screen-space image, with $\lambda_i$ the screen-space barycentric weights of Definition 2.3:
$$
\frac{1}{w_c(x,y)} = \sum_i \lambda_i\frac{1}{w_{c,i}}, \qquad \frac{\phi(x,y)}{w_c(x,y)} = \sum_i\lambda_i\frac{\phi_i}{w_{c,i}}, \qquad \phi(x,y) = \left.\sum_i\lambda_i\frac{\phi_i}{w_{c,i}}\right/\sum_i\lambda_i\frac{1}{w_{c,i}}.
$$

*Proof.* Parametrize the (flat) triangle by object-space barycentric $\mu\in\Delta^2$: $q(\mu)=\sum_i\mu_iq_i$, $\phi(\mu)=\sum_i\mu_i\phi_i$ — both affine in $\mu$ since the triangle is planar and $\mu$ is precisely the coordinate system linear on that plane. Apply $M_{\text{proj}}$: linearity plus $\sum\mu_i=1$ gives $M_{\text{proj}}\tilde q(\mu)=\sum_i\mu_i\tilde c_i$, so
$$
w_c(\mu)=\sum_i\mu_iw_{c,i}\ \ (\text{linear in }\mu), \qquad x_n(\mu) = \Big(\sum_i\mu_ix_{c,i}\Big)\Big/\Big(\sum_i\mu_iw_{c,i}\Big).
$$
This is a ratio of two $\mu$-linear functions — a fractional-linear (Möbius) reparametrization $\mu\mapsto\lambda$, where $\lambda$ is precisely the screen-space barycentric weight (Definition 2.3, since $\lambda$ is read directly off $(x_n,y_n)$). Rather than inverting this map, evaluate $\phi(\mu)/w_c(\mu)$ directly:
$$
\frac{\phi(\mu)}{w_c(\mu)} = \frac{\sum_i\mu_i\phi_i}{\sum_i\mu_iw_{c,i}}.
$$
The substitution
$$
\mu_i = \frac{\lambda_i/w_{c,i}}{\sum_j\lambda_j/w_{c,j}}
\tag{$\dagger$}
$$
is the inverse of $\mu\mapsto\lambda$: check $\sum_i\mu_i = 1$ from ($\dagger$) directly, and $\lambda=e_i\Rightarrow\mu=e_i$ by substitution. Plugging ($\dagger$) into $\phi(\mu)=\sum_i\mu_i\phi_i$:
$$
\phi = \frac{\sum_i(\lambda_i/w_{c,i})\phi_i}{\sum_i\lambda_i/w_{c,i}},
$$
whose denominator is $1/w_c(x,y)$ and numerator is $\phi/w_c(x,y)$, both interpolated linearly in $\lambda_i$ — the claimed identities. $\blacksquare$

*Corollary.* Taking $\phi=z_{\text{cam}}$ (or $\phi=z_n$, itself affine in $z_{\text{cam}}$) specializes Proposition 3.2 to perspective-correct depth. Many implementations store/compare $z_n\in[-1,1]$ rather than $z_{\text{cam}}$ directly (relevant to §5's precision analysis).

### 3.4 Order-independence

**Proposition 3.3.** For opaque primitives, the value of $D(i,j)$ after processing a set of primitives $\{P_1,\dots,P_m\}$ is independent of processing order. *Proof.* $D(i,j) = \min\{z_k(i,j) : P_k \text{ covers } (i,j)\}\cup\{z_{\text{far}}\}$ by induction on the update rule (Definition 3.1) — each update replaces $D(i,j)$ with $\min(D(i,j),z)$, and $\min$ is associative and commutative, so the final value is independent of the order in which the finitely many candidate $z$-values are folded in. $\blacksquare$

### 3.5 Contrast with ray tracing

$$
\textbf{Ray tracing:}\quad \forall\,\text{pixel}:\ t^\ast = \min_{\text{primitive}} t(\text{primitive}) \qquad\qquad \textbf{Rasterization:}\quad \forall\,\text{primitive}:\ \forall\,(i,j)\in\text{footprint}:\ D(i,j)\leftarrow\min(D(i,j),z).
$$

The two loop nests are transposes of one another (pixel-outer-primitive-inner vs.\ primitive-outer-pixel-inner); Proposition 3.3 is exactly what licenses the transpose — it guarantees the same $\min$ regardless of fold order. Consequence: rasterization has no native "second-closest surface" (the buffer only remembers the running $\min$), whereas ray tracing's recursive ray tree is a direct consequence of computing $t^\ast$ analytically, on demand, for arbitrary (including secondary) rays.

---

## 4. Full pipeline: order of operations

1. **Per frame:** $D(i,j)\leftarrow z_{\text{far}}\;\forall(i,j)$; clear color buffer.
2. **Per object $k$:** compose $M=M_{\text{viewport}}M_{\text{proj}}M_{\text{world}\to\text{cam}}M_{\text{obj}\to\text{world}}^{(k)}$ (Proposition 1.5).
3. **Per primitive** (triangle; each $Q$'s two triangles; each tessellated-sphere triangle; or $S$ itself if implicit, §2.5):
   a. Transform vertices by $M$ and $\Pi$; retain $w_c$ per vertex (§3.3).
   b. **Clipping:** discard/subdivide where $w_c\le0$ or outside the frustum, in clip space, before $\Pi$ — $\Pi$ is undefined/sign-flipping at $w_c\le0$.
   c. **Backface culling (optional):** discard by sign of $2\mathcal A$ (Definition 2.3) under a fixed winding convention.
   d. Bounding box (§2.2); evaluate $E_{ij}$ (Definition 2.1) per candidate sample; apply fill rule (Proposition 2.5).
   e. Perspective-correct $z,\phi$ per covered sample (Proposition 3.2).
   f. Depth test (Definition 3.1); update $D,\mathrm{Color}$ on pass.
4. **Per frame, end:** color buffer is the image.

By Proposition 3.3, step 2–3's object/primitive order is immaterial for opaque primitives; transparency blending reintroduces order-dependence and is excluded from that guarantee.

---

## 5. Edge cases, derived

**Proposition 5.1 (Depth precision is $\propto 1/s$, concentrated near $n$).** Let $s:=-z_{\text{cam}}\in[n,f]$. Then
$$
z_n(s) = \frac{f+n}{f-n} - \frac{2fn}{f-n}\cdot\frac1s,
$$
and for $f\gg n$: $z_n(2n)-z_n(n) \approx 1$ out of total range $z_n(f)-z_n(n)=2$ — i.e. $\approx\!\tfrac12$ the buffer's representable range is consumed by $s\in[n,2n]$ alone, independent of $f$.

*Proof.* From Definition 1.2, $z_c = -\frac{f+n}{f-n}z_{\text{cam}}-\frac{2fn}{f-n}$, $w_c=-z_{\text{cam}}=s$, so $z_n=z_c/w_c$ gives the stated formula by direct substitution ($z_{\text{cam}}=-s$). Endpoints: $z_n(n) = \frac{(f+n)-2f}{f-n} = -1$; $z_n(f) = \frac{(f+n)-2n}{f-n} = 1$. At $s=2n$: $z_n(2n) = \frac{f+n}{f-n} - \frac{fn}{n(f-n)} = \frac{f+n-f}{f-n} = \frac{n}{f-n} \approx \frac{n}{f}\to0$ as $f\gg n$. So $z_n(2n)-z_n(n)\approx0-(-1)=1$, versus total range $2$. $\blacksquare$

*Corollary (z-fighting, reverse-$z$).* The remaining $\approx\!\tfrac12$ range is stretched over $s\in[2n,f]$, so two surfaces near $f$ can share a stored $z_n$ despite world-space separation. Since the $[n,2n]$-consumes-half result is $f$-independent, raising $n$ redistributes precision at every subsequent scale; lowering $f$ only reclaims already-scarce far-end range. Storing/comparing $1/s$ directly (reverse-$z$) matches IEEE-754's float density (denser near $0$) to the same near-$n$ concentration requirement, rather than composing two nonlinear remaps.

**Proposition 5.2 (Degeneracy $\Leftrightarrow$ linear dependence).** $2\mathcal A = \det\begin{pmatrix}x_1{-}x_0 & x_2{-}x_0\\ y_1{-}y_0 & y_2{-}y_0\end{pmatrix}$, and $2\mathcal A=0 \Leftrightarrow \{p_1{-}p_0,\,p_2{-}p_0\}$ linearly dependent $\Leftrightarrow p_0,p_1,p_2$ collinear. *Proof.* First equality: expand $E_{12}(p_0)$ (Definition 2.1) directly. Second: standard fact, two vectors in $\mathbb R^2$ dependent $\Leftrightarrow$ determinant $=0$. $\blacksquare$ Since $2\mathcal A$ is $\lambda_i$'s denominator (Definition 2.3), degeneracy forces $0/0$, not a small-but-valid value — must be tested structurally, post-transform (object-space non-degeneracy does not imply screen-space non-degeneracy: an edge-on view drops the pipeline's rank on that direction). $Q$'s degeneracy is the same criterion applied to $(e_1,e_2)$: $\det(e_1,e_2)=0\Leftrightarrow e_1\parallel e_2$.

**Remark 5.3 ($w_c\le0$ clipping).** $\Pi$ undefined/sign-flipping at $w_c\le0\Rightarrow$ clip in clip space (against $w_c=\varepsilon$) before $\Pi$, not after.

**Remark 5.4 (Scale disparity).** Cancellation error in $E_{ij}$ (Definition 2.1) and in $z_{\text{cam}}$ grows with the ratio of primitive scales sharing a scene, for the same fixed-precision-arithmetic reason as Proposition 5.1; mitigated by camera-relative coordinates or multi-pass depth ranges.

**Remark 5.5 (Shared-edge numerical drift).** Independent floating-point evaluation of $E_{ij}$ by two triangles sharing an edge can disagree in the last bits even under a correct fill rule (Proposition 2.5 is combinatorial, not numerical); mitigated by snapping vertices to a fixed sub-pixel grid before rasterizing, guaranteeing bit-identical shared-vertex results.

**Remark 5.6 (Sphere-specific).** Tessellated: ordinary mesh-approximation tradeoff (triangle count vs.\ silhouette accuracy), no depth-buffer-specific issue. Implicit (Corollary 1.11): coverage remains discretely sampled (§2.3's aliasing considerations apply), depth is exact per covered pixel (root of the quadratic in Proposition 1.10(iii)) — $D$ compares whatever $z$ it's given regardless of source, so implicit and tessellated primitives coexist in one buffer with no special-casing at the comparison step.

---

## 6. Bridging to the ray tracer

Shared front end: §1's affine/projective pipeline down to screen space is reusable scene-setup code regardless of algorithm. Divergence at visibility: ray tracing resolves per-ray via Proposition 1.10(iii)'s $t^\ast$, no persistent state; rasterization resolves per-primitive via §2's coverage feeding the persistent $D$ (Proposition 3.3), making primitive order immaterial. Triangles/parallelograms: closed, planar computation (Propositions 1.9, 2.2, 3.2). Spheres: tessellate into that same machinery (approximate), or stay implicit and let coverage come from Corollary 1.11's conic while depth comes from Proposition 1.10(iii) — literally the ray tracer's intersection routine, invoked per covered pixel rather than per image pixel, which is the most direct reuse of existing code between the two renderers.
