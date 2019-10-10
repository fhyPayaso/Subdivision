#include "StdAfx.h"
#include ".\meshsubdivision.h"

//--------------------------------------------------------------
#define PI 3.1415926536
#include <vector>

Mesh3D *MeshSubdivision::Doo_Sabin()
{
	if (m_pmesh == NULL)
	{
		return NULL;
	}

	Mesh3D *New_mesh = new Mesh3D;

	////////////////////////////////////////////////

	//your implmentation
	//New_mesh:  subdivisioned mesh
	//m_pmesh:  original mesh

	// insert all vertex

	PTR_FACE_LIST face_list = m_pmesh->get_faces_list();
	FACE_ITER fiter = face_list->begin();
	float alpha;

	std::vector<int> vert_id_list;
	vert_id_list.assign(m_pmesh->get_num_of_edges_list(), 0);

	//==================
	// YOUR CODE
	//=================

	// for each face, compute new vertex, insert vertex 对于旧网格的每个面，计算新的顶点，插入新网格
	// and insert F-faces 插入这些新顶点构成的面相邻面
	// save the id of these newly inserted vertex , so they can  保存新顶点的id, 使得这些id能被原始网格 的边和顶点访问
	// be acquired using the face and vertex

	int dd = New_mesh->get_num_of_vertices_list();

	for (; fiter != face_list->end(); fiter++)
	{
		//对每一个面
		int n = (*fiter)->valence;
		HE_edge *anchor_edge = (*fiter)->edge;
		// as anchor edge HE_edge * temp_edge;
		VERTEX_LIST v_list;
		float new_x, new_y, new_z;
		//遍历当前面的所有边，计算新的顶点，保存 在v_list里;同时保存在一个数组里，通过原始网格的边可以访问到新的顶点，后面建立V-face 和 E-face 时使用。
		// one face is ok, New_mesh->insert_face(v_list); // F-faces
	}

	//=================
	// YOUR CODE
	//=================

	//
	//search all vertex, construct V-faces
	//

	//===============
	// YOUR CODE
	//===============

	//
	// search all edges, to build E-faces
	//

	//======================
	//  YOUR CODE
	//======================

	/////////////////////////////////////////////////

	New_mesh->update_mesh();

	delete m_pmesh;
	m_pmesh = NULL;
	return New_mesh;
}

//--------------------------------------------------------------

Mesh3D *MeshSubdivision::Catmull_Clark()
{

	if (m_pmesh == NULL)
	{
		return NULL;
	}
}
