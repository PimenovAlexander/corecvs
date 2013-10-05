#include <stdlib.h>

#include "global.h"
#include "gtswrapper.h"
#include <gts.h>

namespace Triangulation
{

/* the file format is the classic GTS file format but only the vertex and
 * edge sections are read. */
static guint load_list (const vector<Point> &points, GPtrArray * vertices)
{
    guint nv = points.size();;

    g_return_val_if_fail (vertices != NULL, 1);

    g_ptr_array_set_size (vertices, nv);
    for (guint i = 1; i <= nv; i++)
    {
        g_ptr_array_index (vertices, i - 1) =
            gts_vertex_new (gts_vertex_class (), points[i-1].x(), points[i-1].y(), 0); // z = 0!
    }
    return 0;
}

static gdouble triangle_cost (GtsTriangle * t, gpointer * data)
{
    gdouble * min_quality = (gdouble *)data[0];
    gdouble * max_area = (gdouble *)data[1];
    gdouble quality = gts_triangle_quality (t);
    gdouble area = gts_triangle_area (t);

    if (quality < *min_quality || area > *max_area)
        return quality;
    return 0.;
}

static gboolean triangle_is_hole (GtsTriangle * t)
{
    GtsEdge * e1, * e2, * e3;
    GtsVertex * v1, * v2, * v3;

    gts_triangle_vertices_edges (t, NULL, &v1, &v2, &v3, &e1, &e2, &e3);

    if ((GTS_IS_CONSTRAINT (e1) && GTS_SEGMENT (e1)->v1 != v1) ||
            (GTS_IS_CONSTRAINT (e2) && GTS_SEGMENT (e2)->v1 != v2) ||
            (GTS_IS_CONSTRAINT (e3) && GTS_SEGMENT (e3)->v1 != v3))
        return TRUE;
    return FALSE;
}

static guint delaunay_remove_holes (GtsSurface * surface)
{
    g_return_val_if_fail (surface != NULL, 0);

    return gts_surface_foreach_face_remove (surface, (GtsFunc) triangle_is_hole, NULL);
}

static void gts_constraint_split (GtsConstraint * c, GtsSurface * s,    GtsFifo * fifo)
{
    GSList * i;
    GtsVertex * v1, * v2;
    GtsEdge * e;

    g_return_if_fail (c != NULL);
    g_return_if_fail (s != NULL);

    v1 = GTS_SEGMENT (c)->v1;
    v2 = GTS_SEGMENT (c)->v2;
    e = GTS_EDGE (c);

    i = e->triangles;
    while (i)
    {
        GtsFace * f = (GtsFace *)i->data;
        if (GTS_IS_FACE (f) && gts_face_has_parent_surface (f, s))
        {
            GtsVertex * v = gts_triangle_vertex_opposite (GTS_TRIANGLE (f), e);
            if (gts_point_orientation (GTS_POINT (v1), GTS_POINT (v2), GTS_POINT (v)) == 0.)
            {
                GSList * j = e->triangles;
                GtsFace * f1 = NULL;
                GtsEdge * e1, * e2;

                /* replaces edges with constraints */
                gts_triangle_vertices_edges (GTS_TRIANGLE (f), e, &v1, &v2, &v, &e, &e1, &e2);
                if (!GTS_IS_CONSTRAINT (e1))
                {
                    GtsEdge * ne1 =    gts_edge_new (GTS_EDGE_CLASS (GTS_OBJECT (c)->klass), v2, v);
                    gts_edge_replace (e1, ne1);
                    gts_object_destroy (GTS_OBJECT (e1));
                    e1 = ne1;
                    if (fifo) gts_fifo_push (fifo, e1);
                }
                if (!GTS_IS_CONSTRAINT (e2))
                {
                                        GtsEdge * ne2 = gts_edge_new (GTS_EDGE_CLASS (GTS_OBJECT (c)->klass), v, v1);
                    gts_edge_replace (e2, ne2);
                    gts_object_destroy (GTS_OBJECT (e2));
                    e2 = ne2;
                    if (fifo) gts_fifo_push (fifo, e2);
                }

                /* look for face opposite */
                while (j && !f1)
                {
                                        if (GTS_IS_FACE (j->data) && gts_face_has_parent_surface ((GtsFace*)j->data, s))
                        f1 = (GtsFace*)j->data;
                    j = j->next;
                }
                if (f1)   /* c is not a boundary of s */
                {
                    GtsEdge * e3, * e4, * e5;
                    GtsVertex * v3;
                                        gts_triangle_vertices_edges (GTS_TRIANGLE (f1), e, &v1, &v2, &v3, &e, &e3, &e4);
                    e5 = gts_edge_new (s->edge_class, v, v3);
                    gts_surface_add_face (s, gts_face_new (s->face_class, e5, e2, e3));
                    gts_surface_add_face (s, gts_face_new (s->face_class, e5, e4, e1));
                    gts_object_destroy (GTS_OBJECT (f1));
                }
                gts_object_destroy (GTS_OBJECT (f));
                return;
            }
        }
        i = i->next;
    }
}

static void add_constraint (GtsConstraint * c, GtsSurface * s)
{
    g_assert (gts_delaunay_add_constraint (s, c) == NULL);
}

static void split_constraint (GtsConstraint * c, gpointer * data)
{
    GtsSurface * s = (GtsSurface *)data[0];
    GtsFifo * fifo = (GtsFifo *)data[1];

    gts_constraint_split (c, s, fifo);
}

static void shuffle_array (GPtrArray * a)
{
    guint i;

    for (i = 0; i < a->len; i++)
    {
        guint j = (gdouble) rand ()*(a->len - 1)/(gdouble) RAND_MAX;
        guint k = (gdouble) rand ()*(a->len - 1)/(gdouble) RAND_MAX;
        gpointer tmp;

        if (j >= a->len) j = a->len - 1;
        if (k >= a->len) k = a->len - 1;

        tmp = g_ptr_array_index (a, j);
        g_ptr_array_index (a, j) = g_ptr_array_index (a, k);
        g_ptr_array_index (a, k) = tmp;
    }
}


static void save_vertex (GtsPoint * p, gpointer * data)
{
    /*(*GTS_OBJECT (p)->klass->write) (GTS_OBJECT (p), (FILE *) data[0]);
    if (!GTS_POINT_CLASS (GTS_OBJECT (p)->klass)->binary)
        fputc ('\n', (FILE *) data[0]);*/
    (*((vector<Point>*)data[5]))[*((guint *) data[1])].x() = p->x;
    (*((vector<Point>*)data[5]))[*((guint *) data[1])].y() = p->y;
    //cout << "Point: (" << (*((guint *) data[1])) << ")"  << p->x << ", " << p->y << endl;
    g_hash_table_insert ((GHashTable*)data[2], p,
                         GUINT_TO_POINTER (++(*((guint *) data[1]))));

}

static void save_edge (GtsSegment * s, gpointer * data)
{
    /*fprintf ((FILE *) data[0], "%u %u",
         GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[2], s->v1)),
         GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[2], s->v2)));
    if (GTS_OBJECT (s)->klass->write)
        (*GTS_OBJECT (s)->klass->write) (GTS_OBJECT (s), (FILE *) data[0]);
    fputc ('\n', (FILE *) data[0]);*/
    std::vector<Point> &points = (*((vector<Point>*)data[5]));
    std::vector<Edge> &edges = (*((vector<Edge>*)data[6]));

    edges[(*((guint *) data[1]))].org = points[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[2], s->v1)) - 1];
    edges[(*((guint *) data[1]))].dest = points[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[2], s->v2)) - 1];
    //cout << "Edge:" << edges[(*((guint *) data[1]))] << endl;
    //<< points[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[2], s->v1)) - 1] << ", "
    //<< points[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[2], s->v2)) - 1] << endl;

        g_hash_table_insert ((GHashTable*)data[3], s, GUINT_TO_POINTER (++(*((guint *) data[1]))));
}

static void save_face (GtsTriangle * t, gpointer * data)
{
    //std::vector<Point> &points = (*((vector<Point>*)data[5]));
    std::vector<Edge> &edges = (*((vector<Edge>*)data[6]));
    std::vector<Edge> &result = (*((vector<Edge>*)data[4]));

    /*fprintf ((FILE*)data[0], "%u %u %u",
         GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e1)),
         GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e2)),
         GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e3)));*/
    //data[4]->
    /*if (GTS_OBJECT (t)->klass->write)
        (*GTS_OBJECT (t)->klass->write) (GTS_OBJECT (t), (FILE*)data[0]);*/
    //fputc ('\n', (FILE*)data[0]);
    /*cout << "Triangle: "
            << edges[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e1)) - 1] << " - "
            << edges[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e2)) - 1] << " - "
            << edges[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e3)) - 1] << endl;*/

    result[(*((guint *) data[1]))++] = edges[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e1)) - 1];
    result[(*((guint *) data[1]))++] = edges[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e2)) - 1];
    result[(*((guint *) data[1]))++] = edges[GPOINTER_TO_UINT (g_hash_table_lookup ((GHashTable*)data[3], t->e3)) - 1];
}

static void gts_surface_save (GtsSurface * s, std::vector<Edge> &result,
                              const int pointsCount, const int edgesCount)
{
        guint n; // counter
        gpointer data[7];
    GHashTable * vindex, * eindex;

    FILE *fptr = stdout;

    g_return_if_fail (s != NULL);
    //g_return_if_fail (fptr != NULL);

    std::vector<Edge> edges(edgesCount);
    std::vector<Point> points(pointsCount);

        data[0] = fptr; // file descriptor only for debug purposes
        data[1] = &n; // counter, should be nulled before each call
        data[2] = vindex = g_hash_table_new (NULL, NULL); // hash table for inner representation
        data[3] = eindex = g_hash_table_new (NULL, NULL); // hash table for inner representation
        data[4] = &result; // triangulation result
        data[5] = &points; // "sorted" array of points for fast point lookup
        data[6] = &edges; // "sorted" array of edges for fast point lookup


    /*if (GTS_OBJECT (s)->klass->write)
        (*GTS_OBJECT (s)->klass->write) (GTS_OBJECT (s), fptr);
    fputc ('\n', fptr);*/

    n = 0;
    gts_surface_foreach_vertex (s, (GtsFunc) save_vertex, data);

    /*if (GTS_POINT_CLASS (s->vertex_class)->binary)
        fputc ('\n', fptr);*/
        n = 0;
    gts_surface_foreach_edge (s, (GtsFunc) save_edge, data);

    n = 0;
    gts_surface_foreach_face (s, (GtsFunc) save_face, data);

    g_hash_table_destroy (vindex);
    g_hash_table_destroy (eindex);
}

std::vector<Edge>    triangulateGts(const vector<Point> &points)
{
    GPtrArray * vertices;
    GtsFifo * edges;
    guint i;
    GtsTriangle * t;
    GtsVertex * v1, * v2, * v3;
    GtsSurface * surface;
    gboolean keep_hull = TRUE;
    gboolean verbose = FALSE;
    gboolean add_constraints = FALSE;
    gboolean remove_holes = FALSE;
    gboolean check_delaunay = FALSE;
    gboolean conform = FALSE;
    gboolean refine = FALSE;
    gboolean split_constraints = FALSE;
    gboolean randomize = FALSE;
    gboolean remove_duplicates = TRUE;
    gint steiner_max = -1;
    gdouble quality = 0., area = G_MAXDOUBLE;

    if (points.size() < 3)
    {
        cout << "(triangulateGts): No enought data!\n";
        return std::vector<Edge>();
    }

    int status = 0;
        //const char * fname = NULL;
    GTimer * timer;

    cout << "Using GTS triangulation\n";


    /* read file => two lists: vertices and constraints */

    edges = gts_fifo_new ();
    vertices = g_ptr_array_new ();
    /*
    if (add_constraints) // the edge class is a GtsConstraintClass
        line = read_list (vertices, edges,
                    GTS_EDGE_CLASS (gts_constraint_class ()),
                    stdin);
    else // the edge class is a "normal" edge: GtsEdgeClass
        line = read_list (vertices, edges,
                    gts_edge_class (),
                    stdin);
    */
    load_list(points, vertices);

    timer = g_timer_new ();
    g_timer_start (timer);

    if (randomize)
        shuffle_array (vertices);

    /* create triangle enclosing all the vertices */
    {
        GSList * list = NULL;
        for (i = 0; i < vertices->len; i++)
            list = g_slist_prepend (list, g_ptr_array_index (vertices, i));
        t = gts_triangle_enclosing (gts_triangle_class (), list, 100.);
        g_slist_free (list);
    }
    gts_triangle_vertices (t, &v1, &v2, &v3);

    /* create surface with one face: the enclosing triangle */
    surface = gts_surface_new (gts_surface_class (),
                               gts_face_class (),
                               gts_edge_class (),
                               gts_vertex_class ());
    gts_surface_add_face (surface, gts_face_new (gts_face_class (),
                          t->e1, t->e2, t->e3));

    /* add vertices */
    for (i = 0; i < vertices->len; i++)
    {
        GtsVertex * v1 = (GtsVertex *)g_ptr_array_index (vertices, i);
        GtsVertex * v = gts_delaunay_add_vertex (surface, v1, NULL);

        g_assert (v != v1);
        if (v != NULL)
        {
            if (!remove_duplicates)
            {
                fprintf (stderr, "delaunay: duplicate vertex (%g,%g) in input file\n",
                         GTS_POINT (v)->x, GTS_POINT (v)->y);
                exit(1); /* Failure */
            }
            else
                gts_vertex_replace (v1, v);
        }
                /*if (fname)
        {
            static guint nf = 1;
            char s[80];
            FILE * fp;

            g_snprintf (s, 80, "%s.%u", fname, nf++);
            fp = fopen (s, "wt");
            gts_surface_write_oogl (surface, fp);
            fclose (fp);

            if (check_delaunay && gts_delaunay_check (surface))
            {
                fprintf (stderr, "delaunay: triangulation is not Delaunay\n");
                exit(1);
            }
                }*/
    }
    g_ptr_array_free (vertices, TRUE);

    /* add remaining constraints */
    if (add_constraints)
        gts_fifo_foreach (edges, (GtsFunc) add_constraint, surface);

    /* destroy enclosing triangle */
    gts_allow_floating_vertices = TRUE;
    gts_object_destroy (GTS_OBJECT (v1));
    gts_object_destroy (GTS_OBJECT (v2));
    gts_object_destroy (GTS_OBJECT (v3));
    gts_allow_floating_vertices = FALSE;

    if (!keep_hull)
        gts_delaunay_remove_hull (surface);

    if (remove_holes)
        delaunay_remove_holes (surface);

    if (split_constraints)
    {
        gpointer data[2];

        data[0] = surface;
        data[1] = edges;
        gts_fifo_foreach (edges, (GtsFunc) split_constraint, data);
    }

    if (conform)
    {
        guint encroached_number =
            gts_delaunay_conform (surface,
                                  steiner_max,
                                  (GtsEncroachFunc) gts_vertex_encroaches_edge,
                                  NULL);
        if (encroached_number == 0 && refine)
        {
            guint unrefined_number;
            gpointer data[2];

            data[0] = &quality;
            data[1] = &area;
            unrefined_number =
                gts_delaunay_refine (surface,
                                     steiner_max,
                                     (GtsEncroachFunc) gts_vertex_encroaches_edge,
                                     NULL,
                                     (GtsKeyFunc) triangle_cost,
                                     data);
            if (verbose && unrefined_number > 0)
                fprintf (stderr,
                         "delaunay: ran out of Steiner points (max: %d) during refinement\n"
                         "%d unrefined faces left\n",
                         steiner_max, unrefined_number);
        }
        else if (verbose && encroached_number > 0)
            fprintf (stderr,
                     "delaunay: ran out of Steiner points (max: %d) during conforming\n"
                     "Delaunay triangulation: %d encroached constraints left\n",
                     steiner_max, encroached_number);
    }
    g_timer_stop (timer);

    if (verbose)
    {
        gts_surface_print_stats (surface, stderr);
        fprintf (stderr, "# Triangulation time: %g s speed: %.0f vertex/s\n",
                 g_timer_elapsed (timer, NULL),
                 gts_surface_vertex_number (surface)/g_timer_elapsed (timer, NULL));
    }
    g_timer_destroy(timer);

    if (check_delaunay && gts_delaunay_check (surface))
    {
        fprintf (stderr, "delaunay: triangulation is not Delaunay\n");
        status = 1; /* failure */
    }

    int trianglesCount;
    int edgesCount;

    {
        GtsSurfaceStats stats;
        gts_surface_stats (surface, &stats);
        /*fprintf (fptr, "%u %u %u",
             stats.edges_per_vertex.n, //vertices
             stats.faces_per_edge.n, // edges
             stats.n_faces); //faces*/
        trianglesCount = stats.n_faces;
        edgesCount = stats.faces_per_edge.n;
    }

    /* write triangulation */
    std::vector<Edge> result(trianglesCount * 3);
    gts_surface_save (surface, result, points.size(), edgesCount);

    gts_fifo_destroy(edges);
    gts_object_destroy (GTS_OBJECT (surface));
    fprintf(stderr, "Cleared\n");

    return result;
}

}
