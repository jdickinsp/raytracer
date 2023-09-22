# Ray Tracing

Ray tracing starts as rays originating from the observer's position into the 2d screen.

It needs to calculate the screen position of the rays to world coordinates. This is the opposite in rasterized graphics, which start from world to screen and then to pixels.

Camera is aligned with world coordinates at position (0,0,0), direction (0, 0, -1), and up vector at (0, 1, 0).


$$
\begin{align}
& d = \frac{1}{tan(\theta_{fov}/2)} \\
& aspect = w / h \\
& Px = Px + 0.5 \\
& Py = Py + 0.5 \\
\end{align}
$$

Ray origin is at the camera position (0, 0, 0).
$$
\begin{align}
& dir.x = aspect * ((2 * P_x / w) - 1) \\
& dir.y = (2 * P_y / h) - 1 \\
& dir.z = d \\
&  \\
\end{align}
$$
The ray direction is a normalized vector.

Transform matrix from screen to world.

$$
A_{m,n} = 
\begin{pmatrix}
a_{1,1} & a_{1,2} & \cdots & a_{1,n} \\
a_{2,1} & a_{2,2} & \cdots & a_{2,n} \\
\vdots & \vdots & \ddots & \vdots \\
a_{m,1} & a_{m,2} & \cdots & a_{m,n} 
\end{pmatrix}
$$

## Light
Light transport algorithms describes the way light is reflected, refracted or diffused on a material surface.

Reflection depends on the surface normal and light ray direction.

Refraction is calculated using Snell's law the surface normal, ray direction and material refraction index.

Diffused lighting on opaque materials can be calculated using the Phong shading model.


Tree of light rays.


Types of lights
1. Directional light. e.g. the sun. Light rays in parallel oriented in a direction.
2. Positional light.
3. Area/Volumeric light.


Types of shading
1. Flat shading. Per face.
2. Gourand shading. Per vertex.
3. Phong shading. Per pixel.


#### Lambert's Cosine Law
$$
cos(\theta) = N \cdot L
$$
The amount of light a surface receives is proportional to the angle between the surface normal and the light direction.

Diffuse surface color
$$
\begin{align}
& color = \frac{\rho}{\pi} * L_i * \cos{\theta} \\
\end{align}
$$

The illumination of an diffuse object with multiple lights is the summation of the effects of each light on the surface.

$$
\begin{align}
& S_p = \sum_{N_i=0}^{N-1} \frac{\rho_d}{\pi} * L_{iN} * \cos({N * L_{N}}) \\
\end{align}
$$


Phong shading covers materials with both diffuse and specular properties.

$$
\begin{align}
& R = 2 (N \cdot L)N - L \\
& Specular \approx (V \cdot R)^{n} \\
& S_p = Diffuse() * K_d + Specular() * K_s \\
\end{align}
$$

#### Refraction

Snell's law models refraction.

$$
\begin{align}
& \frac{sin(\theta_1)}{sin(\theta_2)} = \frac{\eta_2}{\eta_1} \\
\end{align}
$$

From there the transmitted vector is calculated.

$$
\begin{align}
& \eta = \frac{\eta_1}{\eta_2} \\
& c_1 = \cos({\theta_1}) = N \cdot I \\
& c_2 = \sqrt{1 - \left(\frac{\eta_1}{\eta_2}\right)^{2}\sin^{2}({\theta_1})} \rightarrow \sqrt{1 - \left(\frac{\eta_1}{\eta_2}\right)^{2}(1-\cos^{2}({\theta_1}))} \\
& T = \eta I + (\eta c_1 - c_2) N \\
\end{align}
$$

#### Fresnel equations

Fresnel models objects that are both reflective and refractive.

$$
\begin{align}
& F_{R\parallel} = \left(\frac{\eta_2 \cos({\theta_1}) - \eta_1 \cos({\theta_2})}{\eta_2 \cos({\theta_1}) + \eta_1 \cos({\theta_2})}\right)^{2} \\
& F_{R\bot} = \left(\frac{\eta_1 \cos({\theta_2}) - \eta_2 \cos({\theta_1})}{\eta_1 \cos({\theta_2}) + \eta_2 \cos({\theta_1})}\right)^{2} \\
& F_R = \frac{1}{2} \left(F_{R\parallel} + F_{R\bot}\right) \\
\end{align}
$$

## Objects

#### Ray
Point at time step.
$$
\begin{align}
& p = l_0 + l * t
\end{align}
$$

#### Plane
An equation of a plane.

$$
\begin{align}
& Ax + By + Cz = D   \\
\end{align}
$$


Intersection of ray in a plane.

$$
\begin{align}
& t = \frac{(p_0 - l_0) \cdot n}{l \cdot n} \\
\end{align}
$$


#### Triangle

In counter clock wise order, the normal is
$$
\begin{align}
& A = v_1 - v_0 \\
& B = v_2 - v_0 \\
& n = \frac{A \times B}{|| A \times B ||} \\
& d = n \cdot A \\
\end{align}
$$

To find the intersection of the ray with the plane that contains the triangle.

$$
\begin{align}
& t = \frac {d - n \cdot l_o} {n \cdot l} \\
& Q = l_o  + l * t \\
\end{align}
$$

Triangle inside out tracking, Q is inside the triangle if the conditions are true.

$$
\begin{align}
& [(B - A) \times (Q - A)] \cdot n \geq  0 \\
& [(C - B) \times (Q - B)] \cdot n \geq  0 \\
& [(A - C) \times (Q - C)] \cdot n \geq  0 \\
\end{align}
$$

Barycentric coordinates are a way to derive any point in a triangle from 3 parameters (u,v,w).
$$
P = Au + Bv + Cw
$$

To derive the barycentric coordinates from the point Q on the triangle.

$$
\begin{align}
& u = \frac {Area(QBC)} {Area(ABC)}, v = \frac {Area(AQC)} {Area(ABC)}, w = \frac {Area(ABQ)} {Area(ABC)} \\
& u = \frac {[(C - B) \times (Q - B)] \cdot n} {[(B - A) \times (C - A)] \cdot n} \\
& v = \frac {[(A - C) \times (Q - C)] \cdot n} {[(B - A) \times (C - A)] \cdot n} \\
& w = \frac {[(B - A) \times (Q - A)] \cdot n} {[(B - A) \times (C - A)] \cdot n} \\
\end{align}
$$

The barycentric normal at point Q is from 

$$
\begin{align}
& N_Q = \frac {u N_a + v N_b + w N_c} {|| u N_a + v N_b + w N_c ||} \\
\end{align}
$$

#### Polygons
- face index array - number of vertices that make up each face.
- vertex index array - the index of the vertices for each face.
- vertex array - The array of vertices that make up a mesh.

Triangulation is easy for simple convex polygons. The amount of triangles that make up a polygon is equal to the number of face edges - 2.

