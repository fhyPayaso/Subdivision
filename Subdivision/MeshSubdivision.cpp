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
        do
		{
			float new_x = 0, new_y = 0, new_z = 0;
			// 当前边所指向的顶点
			HE_vert* cur_edge_vert = temp_edge->vert;
            // 从当前边开始遍历顶点，计算新顶点的坐标
			HE_edge* alpha_edge = temp_edge;

			int i = 0;
			do
            {
                alpha = (i == 0 ? n + 5 : 3 + 2 * std::cos(2 * PI * i / n)) / (4 * n * 1.0);

                new_x += alpha * cur_edge_vert->x;
				new_y += alpha * cur_edge_vert->y;
				new_z += alpha * cur_edge_vert->z;

			    i++;
                // 迭代计算下一个顶点相对新顶点的权重
                alpha_edge = alpha_edge->next;
                cur_edge_vert = alpha_edge->vert;
            }while (alpha_edge != temp_edge)

			// 将计算出的新顶点插入新网格
			HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
			// 半边和新顶点的数量是一一对应的，建立旧网格半边与新顶点的id对应关系
			vert_id_list[temp_edge->id] = new_vert->id;

			// 记录当前F-face所对应的点
			v_list->push_back(new_vert);

			temp_edge = temp_edge->next;
		}while (temp_edge != anchor_edge);
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

    Mesh3D *New_mesh = new Mesh3D;
	// 初始化id映射数组
    std::vector<int> f_vert_id_list ,e_vert_id_list ,v_vert_id_list;
    f_vert_id_list.assign(m_pmesh->get_num_of_faces_list(),0);
    e_vert_id_list.assign(m_pmesh->get_num_of_edges_list(),0);
    v_vert_id_list.assign(m_pmesh->get_num_of_vertices_list(),0);
	//==================================================
	// Step 1 : 对于旧网格每个面，生成新的面顶点，并插入新网格
	//==================================================
    PTR_FACE_LIST face_list = m_pmesh->get_faces_list();
    for (FACE_ITER fiter = face_list->begin(); fiter != face_list->end(); fiter++)
    {
        float new_x = 0, new_y = 0, new_z = 0;
        HE_edge* firstEdge = (*fiter)->edge;
        HE_edge* curEdge = firstEdge;
        int n = (*fiter)->valence;
        do
        {
            HE_vert* curVert = curEdge->vert;
            new_x += curVert->x * 1.0 / n;
            new_y += curVert->y * 1.0 / n;
            new_z += curVert->z * 1.0 / n;

            curEdge = curEdge->next;
        }while (firstEdge != curEdge);
        // 将计算出的面顶点插入新网格
        HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
        // 存储面与新面顶点之间的id对应关系
        f_vert_id_list[(*fiter)->id] = new_vert->id;
    }

    //==================================================
    // Step 2 : 对于旧网格每个边，生成新的边顶点，并插入新网格
    //==================================================
    PTR_EDGE_LIST edge_list = m_pmesh->get_edges_list();
    // 初始化标记位
    for(EDGE_ITER eiter = edge_list->begin(); eiter != edge_list->end() ; eiter++)
    {
        (*eiter)->tag = true;
    }
    for(EDGE_ITER eiter = edge_list->begin(); eiter != edge_list->end() ; eiter++)
    {
        if((*eiter)->tag)
        {
            float new_x = 0, new_y = 0, new_z = 0;
            HE_edge* curEdge = *eiter;
            HE_edge* pairEdge = curEdge->pair;
            // 先获取旧网格的顶点
            HE_vert* left_vert = curEdge->vert;
            HE_vert* right_vert = pairEdge->vert;
            // 获取新的面顶点
            HE_vert* up_vert =  New_mesh->get_vertex(f_vert_id_list[curEdge->face->id]);
            HE_vert* down_vert =  New_mesh->get_vertex(f_vert_id_list[pairEdge->face->id]);
            // 计算新顶点
            new_x = (left_vert->x + right_vert->x + up_vert->x + down_vert->x)/4;
            new_y = (left_vert->y + right_vert->y + up_vert->y + down_vert->y)/4;
            new_z = (left_vert->z + right_vert->z + up_vert->z + down_vert->z)/4;
            // 将计算出的边顶点插入新网格
            HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
            // 存储边与新边顶点之间的id对应关系
            e_vert_id_list[curEdge->id] = new_vert->id;
            e_vert_id_list[pairEdge->id] = new_vert->id;
            // 两个半边只生成一个边顶点
            curEdge->tag = false;
            pairEdge->tag = false;
        }
    }
    //===================================================
    // Step 3 : 对于旧网格每个顶点，生成新的顶点，并插入新网格
    //          对应公式 => (Q + 2R + (n−3)S) / n
    //===================================================
    PTR_VERTEX_LIST vert_list = m_pmesh->get_vertices_list();
    for(VERTEX_ITER viter = vert_list->begin() ;viter != vert_list->end() ;viter++)
    {
        // Q是该顶点相邻的面的新顶点的平均
        HE_vert* Q = new HE_vert(0.0,0.0,0.0);
        // R是该顶点的相邻边的中点的平均
        HE_vert* R = new HE_vert(0.0,0.0,0.0);
        // S为原顶点
        HE_vert* S = *viter;
        //n是该顶点相邻的边的数目
        int n = S->degree;
        // 构造Q和R
        HE_edge* firstEdge = S->edge;
        HE_edge* curEdge = S->edge;
        do
        {
            // 获取新的面顶点和边顶点
            HE_vert* f_vert = New_mesh->get_vertex(f_vert_id_list[curEdge->face->id]);
            HE_vert* e_vert = New_mesh->get_vertex(e_vert_id_list[curEdge->id]);
            // 累加坐标值
            Q->x += f_vert->x; Q->y += f_vert->y; Q->z += f_vert->z;
            R->x += e_vert->x; R->y += e_vert->y; R->z += e_vert->z;
            curEdge = curEdge->pair->next;
        }while (curEdge != firstEdge);
        // 计算新顶点
        Q->x /= n; Q->y /= n; Q->z /= n;
        R->x /= n; R->y /= n; R->z /= n;
        float new_x = 0, new_y = 0, new_z = 0;
        new_x = (Q->x + 2 * R->x + (n-3) * S->x) / n;
        new_y = (Q->y + 2 * R->y + (n-3) * S->y) / n;
        new_z = (Q->z + 2 * R->z + (n-3) * S->z) / n;
        // 插入新顶点
        HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
        // 存储旧顶点和新顶点之间的id对应关系
        v_vert_id_list[S->id] = new_vert->id;
    }
    //===================================================
    // Step 4 : 开始构造面
    //          (1)一个新的面顶点和与包围该面的新的边顶点连接
    //          (2)一个新的顶点顶点和与该顶点相邻的新的边顶点连接
    //===================================================
    for (FACE_ITER fiter = face_list->begin(); fiter != face_list->end(); fiter++)
    {
        HE_vert* f_vert = New_mesh->get_vertex(f_vert_id_list[(*fiter)->id]);
        HE_edge* firstEdge = (*fiter)->edge;
        HE_edge* curEdge = (*fiter)->edge;
        VERTEX_LIST*  v_list = new VERTEX_LIST;
        do
        {
            // 构造并插入新的面
            v_list->push_back(New_mesh->get_vertex(e_vert_id_list[curEdge->id]));
            v_list->push_back(New_mesh->get_vertex(v_vert_id_list[curEdge->vert->id]));
            v_list->push_back(New_mesh->get_vertex(e_vert_id_list[curEdge->next->id]));
            v_list->push_back(f_vert);
            New_mesh->insert_face(*v_list);
            v_list->clear();

            curEdge = curEdge->next;
        }while (firstEdge != curEdge);
    }
    /////////////////////////////////////////////////
    New_mesh->update_mesh();

    delete m_pmesh;
    m_pmesh = NULL;
    return New_mesh;
}
