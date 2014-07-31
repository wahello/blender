#ifndef MANTA_H
#define MANTA_H
#include "FLUID_3D.h"
#include "zlib.h"
#include "../../../source/blender/makesdna/DNA_scene_types.h"
#include "../../../source/blender/makesdna/DNA_modifier_types.h"
#include "../../../source/blender/makesdna/DNA_smoke_types.h"
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <pthread.h>
#include <Python.h>
#include <vector>
#include "../../../source/blender/blenlib/BLI_path_util.h"
//#include "../../../source/blender/blenlib/BLI_fileops.h"
//#include "../../../source/blender/python/manta_pp/pwrapper/pymain.cpp"

struct manta_arg_struct {
	std::string filepath;
	int frame_num;
};

void runMantaScript(vector<string>& args);//defined in manta_pp/pwrapper/pymain.cpp

extern "C" bool manta_check_grid_size(struct FLUID_3D *fluid, int dimX, int dimY, int dimZ);

extern "C" void read_mantaflow_sim(struct FLUID_3D *fluid, char *name);

void indent_ss(stringstream& ss, int indent);

void manta_gen_noise(stringstream& ss, char* solver, int indent, char *noise, int seed, bool load, bool clamp, float clampNeg, float clampPos, float valScale, float valOffset, float timeAnim);

void manta_solve_pressure(stringstream& ss, char *flags, char *vel, char *pressure, bool useResNorms, int openBound, int solver_res,float cgMaxIterFac=1.0, float cgAccuracy = 0.01);

void manta_advect_SemiLagr(stringstream& ss, int indent, char *flags, char *vel, char *grid, int order);

/*create solver, handle 2D case*/
void manta_create_solver(stringstream& ss, char *name, char *nick, char *grid_size_name, int x_res, int y_res, int z_res, int dim);

inline bool file_exists (const std::string& name);

/*blender transforms obj coords to [-1,1]. This method transforms them back*/
void add_mesh_transform_method(stringstream& ss);

void manta_cache_path(char *filepath);

static bool manta_sim_running=true;

//void BLI_dir_create_recursive(const char *filepath);
void create_manta_folder();

void *run_manta_scene_thread(void *threadid);

void *run_manta_sim_thread(void *threadid);

void run_manta_scene(char * filepath);

void generate_manta_sim_file(Scene *scene, SmokeModifierData *smd);

void manta_sim_step(int frame);

#endif /* MANTA_H */

