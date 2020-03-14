#include "properties.h"
#include "render.h"
#include "objects.h"

 
std::pair<float, float> IntersectRaySphere(Point &O, Vector &D, Sphere &sphere)
{
    float r = sphere.radius;
    Vector OC = O - sphere.center;

    float k1 = D * D;
    float k2 = 2 * (OC * D);
    float k3 = OC * OC - r*r;

    float discriminant = k2*k2 - 4*k1*k3;
    if(discriminant < 0)
        return std::make_pair(INF, INF);

    float t1 = (-k2 + sqrt(discriminant)) / (2*k1);
    float t2 = (-k2 - sqrt(discriminant)) / (2*k1);
    return std::make_pair(t1, t2);

}


Vector ReflectRay(Vector &R, Vector &N) {
	float k = N * R;
    Vector C = N * 2;
    C = C * k;
    Vector L = R * (-1);
    C = C + L;
    return C;
}


bool ClosestIntersection(Point &O, Vector &D, float t_min, float t_max, float &clo_t, Sphere &sp)
{
    float closest_t = INF;
    bool closest_sphere = false;
    Sphere sphere;
    for(Sphere &sph: objects)
    {
        std::pair<float, float> t = IntersectRaySphere(O, D, sph);
        if (t.first >= t_min and t.first <= t_max and t.first < closest_t)
        {
            closest_t = t.first;
            closest_sphere = true;
            sphere = sph;
        }
        if (t.second >= t_min and t.second <= t_max and t.second < closest_t)
        {
            closest_t = t.second;
            closest_sphere = true;
			sphere = sph;        
        }
    }

    clo_t = closest_t;
    sp = sphere;

    return closest_sphere;
}


float ComputeLighting(Point &P, Vector &N, Vector &V, int specular)
{
    float i = 0.0;
    for(Light & l : lights)
    {
        if (l.type == 0)
            i += l.intensity;
        else
		{
			Vector L(0, 0, 0);
			float t_max;
            if (l.type == 1)
            {
                L = l.position - P;
                t_max = 1;
            }
            else
            {
                L = l.direction;
                t_max = INF;
            }

           
            float closest_t;
    		Sphere sphere;
            if(ClosestIntersection(P, L, EPSILON, t_max, closest_t, sphere))
                continue;


            float k = N * L;
            k /= (N.norm()*L.norm());
            if (k > 0)
                i += l.intensity * k;


            if (specular != -1)
            {
            	Vector R = ReflectRay(L, N);

                k = R * V;
                if (k > 0)
                    i += l.intensity * pow(k / (R.norm() * V.norm()), specular);
           	}

        }
    }
    return i;
}


Color TraceRay(Point &O, Vector &D, float t_min, float t_max, int depth)
{
    float closest_t;
    Sphere sphere;

    if(!ClosestIntersection(O, D, t_min, t_max, closest_t, sphere))
        return Back_ground;

    Point P = D.to_point(closest_t);
    P = O + P;
    Vector N = P - sphere.center;
    N = N / N.norm();
    Vector V = D * (-1);
    Color local_color = sphere.color * ComputeLighting(P, N, V, sphere.specular);

    float r = sphere.reflective;
    if(depth <= 0 || r <= 0)
    	return local_color;

    Vector R = ReflectRay(V, N);
    Color reflected_color = TraceRay(P, R, EPSILON, INF, depth - 1);

    return local_color * (1 - r) + reflected_color * r;

}


void render(std::vector<uint32_t> &image, Camera &camera)
{

	for(int i = (-1)*(int)HEIGHT/2; i < HEIGHT/2; ++i)
	{
    	for(int j = (-1)*(int)WIDTH/2; j < WIDTH/2; ++j)
    	{
        	Vector D = camera.point_to_vector(i, j);
        	Color color = TraceRay(camera.O, D, 1, INF, RECURSION_DEPTH);
        	image[(i+HEIGHT/2)*WIDTH + (j+WIDTH/2)] = color.hex();
    	}
	}

   return;
}


void build_image(std::vector<uint32_t> &image, int sceneId)
{

	switch(sceneId)
	{
		case 0:
		{

			objects.emplace_back(Point(2, -1, 12), 1, 10, 0.05, Color(70,30,100));
		    objects.emplace_back(Point(0, 0, 17), 4, 200, 0.2, Color(0,0,200));
		    objects.emplace_back(Point(-8, -9, 16), 6, 400, 0.6, Color(60,60,60));
		    objects.emplace_back(Point(-10, 10, 30), 16, 300, 0.3, Color(0,128,0));


		    lights.push_back(Light(1, 0.8, Point(15,10,0)));
		    lights.push_back(Light(2, 0.2, Vector(1,1,-5)));
		    lights.push_back(Light(0, 0.1));



		    Camera camera(Point(0,0,0), Vector(0,0,1), 60);

		    render(image, camera);

		    std::cout << "Scene 0" << std::endl;

		    break;
		}
		case 1:
		{
			std::cout << "Scene 1" << std::endl;

			break;
		}
		case 2:
		{
			std::cout << "Scene 2" << std::endl;

			break;
		}
		case 3:
		{
			std::cout << "Scene 3" << std::endl;

			break;
		}
		default:
		{
			std::cout << "Bad scene" << std::endl;

			break;
		}
	}

    return;
}