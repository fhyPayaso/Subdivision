#include "StdAfx.h"
#include ".\meshsubdivision.h"
#include "Mesh3D.h"

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

	std::vector<int> vert_id_list;
	vert_id_list.assign(m_pmesh->get_num_of_edges_list(),0);
	// count of vertex
	// int dd = New_mesh->get_num_of_vertices_list();

	//==================
	// YOUR CODE
	//=================

	// for each face, compute new vertex, insert vertex 对于旧网格的每个面，计算新的顶点，插入新网格
	// and insert F-faces 插入这些新顶点构成的面相邻面
	// save the id of these newly inserted vertex , so they can  保存新顶点的id, 使得这些id能被原始网格 的边和顶点访问
	// be acquired using the face and vertex

	// 获取旧网格的全部面
	PTR_FACE_LIST face_list = m_pmesh->get_faces_list();
	FACE_ITER fiter = face_list->begin();
	// 遍历每个面
	for (; fiter != face_list->end(); fiter++)
	{
		// 获取边的数量
		int n = (*fiter)->valence;
		float alpha;
		// 记录当前面的第一条边
		HE_edge* anchor_edge = (*fiter)->edge;
		HE_edge* temp_edge = (*fiter)->edge;
		// 遍历当前面的所有边
		VERTEX_LIST*  v_list = new VERTEX_LIST;
        while (temp_edge != anchor_edge);
		{
			float new_x = 0, new_y = 0, new_z = 0;
			// 当前边所指向的顶点
			HE_vert* cur_edge_vert = temp_edge->vert;
            // 从当前边开始遍历顶点，计算新顶点的坐标
			HE_edge* alpha_edge = temp_edge;

			int i = 0;
			while (alpha_edge != temp_edge)
            {
                alpha = (i == 0 ? n + 5 : 3 + 2 * std::cos(2 * PI * i / n)) / (4 * n * 1.0);

                new_x += alpha * cur_edge_vert->x;
				new_y += alpha * cur_edge_vert->y;
				new_z += alpha * cur_edge_vert->z;

			    i++;
                // 迭代计算下一个顶点相对新顶点的权重
                alpha_edge = alpha_edge->next;
                cur_edge_vert = alpha_edge->vert;
            }

			// 将计算出的新顶点插入新网格
			HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
			// 半边和新顶点的数量是一一对应的，建立旧网格半边与新顶点的id对应关系
			vert_id_list[temp_edge->id] = new_vert->id;

			// 记录当前F-face所对应的点
			v_list->push_back(new_vert);

			temp_edge = temp_edge->next;
		}
		// 插入当前面所对应的F-faces
		New_mesh->insert_face(*v_list);
		delete v_list;
	}

	//
	//search all vertex, construct V-faces
	//

	PTR_VERTEX_LIST vert_list = m_pmesh->get_vertices_list();
    VERTEX_ITER viter = vert_list->begin();
    // 遍历每个点
    for(;viter != vert_list->end() ;viter++)
    {
        VERTEX_LIST*  v_list = new VERTEX_LIST;
        // 拿到指向该顶点的其中一条半边
        HE_edge* first_edge = (*viter)->edge->pair;
        HE_edge* temp_edge = first_edge;
        // 遍历所有指向该顶点的半边
        while (temp_edge != first_edge)
        {
            // 获取这个半边所对应的新顶点id
            int vert_id = vert_id_list[temp_edge->id];
            v_list->push_back(New_mesh->get_vertex(vert_id));
            temp_edge = temp_edge->next->pair;
        }
        // 插入当前点所对应的V-faces
        New_mesh->insert_face(*v_list);
        delete v_list;
    }

	//
	// search all edges, to build E-faces
	// =================================
	//               | | <----------
	//            d  | |  a
	//            c  | |  b
	//   ----------> | |
	// =================================
	//

	PTR_EDGE_LIST edge_list = m_pmesh->get_edges_list();
    EDGE_ITER eiter = edge_list->begin();
    for(; eiter != edge_list->end() ; eiter++)
    {
        HE_edge* curEdge = *eiter;
        HE_edge* nextEdge = curEdge->next;
        HE_edge* nextEdgePair = nextEdge->pair;
        HE_edge* nextEdgePairPre = nextEdgePair;
        // 寻找前一条边
        while (nextEdgePairPre->next != nextEdgePair)
            nextEdgePairPre = nextEdgePairPre->next;

        // 构造新点
        VERTEX_LIST*  v_list = new VERTEX_LIST;
        v_list->push_back(New_mesh->get_vertex(vert_id_list[curEdge->id]));
        v_list->push_back(New_mesh->get_vertex(vert_id_list[nextEdge->id]));
        v_list->push_back(New_mesh->get_vertex(vert_id_list[nextEdgePairPre->id]));
        v_list->push_back(New_mesh->get_vertex(vert_id_list[nextEdgePair->id]));
        // 插入当前边所对应的E-faces
        New_mesh->insert_face(*v_list);
        delete v_list;
    }

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
