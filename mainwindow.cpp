#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

/* **** début de la partie à compléter **** */
void MainWindow::showEdgeSelection(MyMesh* _mesh)
{
    // on réinitialise les couleurs de tout le maillage
    resetAllColorsAndThickness(_mesh);

    if(edgeSelection > -1 && edgeSelection < _mesh->n_edges() ) {
        EdgeHandle edgeHandle = _mesh->edge_handle(edgeSelection);
        _mesh->set_color(edgeHandle, MyMesh::Color(0, 255, 0));
        _mesh->data(edgeHandle).thickness = 3;

        HalfedgeHandle heh0 = _mesh->halfedge_handle(edgeHandle, 0); // la première demi-arête
        HalfedgeHandle heh1 = _mesh->halfedge_handle(edgeHandle, 1); // la seconde demi-arête

        VertexHandle v = _mesh->to_vertex_handle(heh0);
        _mesh->set_color(v, MyMesh::Color(0,255,0));
        _mesh->data(v).thickness = 7;
        v = _mesh->to_vertex_handle(heh1);
        _mesh->set_color(v, MyMesh::Color(0,255,0));
        _mesh->data(v).thickness = 7;
    }

    // on affiche le nouveau maillage
    displayMesh(_mesh);
}

void MainWindow::collapseEdge(MyMesh* _mesh, int edgeID)
{
    /* **** à compléter ! (Partie 1) ****
     *
     * cette fonction utilise l'opérateur collapse pour supprimer l'arête d'index edgeID
     * Attention à ne pas oublier garbage_collection() !
     */

    if(edgeID > -1 && edgeID < _mesh->n_edges()) {
        EdgeHandle edgeHandle = _mesh->edge_handle(edgeID);
        bool swap=false;
        HalfedgeHandle heh0 = _mesh->halfedge_handle(edgeHandle, 0); // la première demi-arête
        VertexHandle v1,v2;

        if (!_mesh->PolyConnectivity::is_collapse_ok(heh0)){
            heh0=_mesh->halfedge_handle(edgeHandle, 1);
            swap=true;
        }
        if (_mesh->PolyConnectivity::is_collapse_ok(heh0)){
            if(swap){
                qDebug() << "needed to swap helfedge" ;
            }
            v1 = _mesh->from_vertex_handle(heh0);
            v2 = _mesh->to_vertex_handle(heh0);
            MyMesh::Point v;

            v= _mesh->point( v1 );
            v+= _mesh->point( v2 );
            v=v/2;
            // Collapse edge
            _mesh->collapse(heh0);
            _mesh->set_point( v2, v);
            // permet de nettoyer le maillage et de garder la cohérence des indices après un collapse
            _mesh->garbage_collection();
        }
    }


}

// fonction pratique pour faire des tirages aléatoires
int randInt(int low, int high){return qrand() % ((high + 1) - low) + low;}

void MainWindow::decimation(MyMesh* _mesh, int percent, QString method)
{
    /* **** à compléter ! (Partie 2 et 3) ****
     * Cette fonction supprime des arêtes jusqu'à atteindre un pourcentage d'arêtes restantes, selon un critère donné
     * percent : pourcentage de l'objet à garder
     * method  : la méthode à utiliser parmis : "Aléatoire", "Par taille", "Par angle", "Par planéité"
     */

    if(method == "Aléatoire")
    {
        int baseNbVertice=_mesh->n_edges(),i=0;
        double nbDecim=baseNbVertice*percent*1.0/100;
        for (i=0; i<nbDecim &&_mesh->n_edges()*1.0/baseNbVertice > percent*1.0/100 ; i++) {
            int rand=randInt(0,_mesh->n_edges()-1);
            collapseEdge(_mesh, rand);
        }
        qDebug() << percent << "pourcent retirer" << i <<"tentatives de colapse" << baseNbVertice-_mesh->n_edges() <<"vertices retirées";

    }
    else if(method == "Par taille")
    {
        int baseNbVertice=_mesh->n_edges(),i=0;
        double nbDecim=baseNbVertice*percent*1.0/100;

        std::vector<float> taille;
        std::vector<int> IDedge;

        // parcours des arêtes
        for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
        {
            EdgeHandle eh = *curEdge;
            MyMesh::HalfedgeHandle heh0 = _mesh->halfedge_handle(eh, 0); // la première demi-arête

            VertexHandle vh0 = _mesh->to_vertex_handle(heh0);
            VertexHandle vh1 = _mesh->from_vertex_handle(heh0);

            OpenMesh::Vec3f test = _mesh->point(vh1) - _mesh->point(vh0);
            float tmpTaille = test.norm();

            taille.push_back(tmpTaille);
            IDedge.push_back(eh.idx());
        }

        for(int i = 0; i<nbDecim ; i++){
            int idPlusPetit = std::min_element(taille.begin(),taille.end()) - taille.begin();

            collapseEdge(_mesh, IDedge.at(idPlusPetit));
            taille.erase(taille.begin() + idPlusPetit);
            IDedge.erase(IDedge.begin() + idPlusPetit);
        }
        qDebug() << percent << "pourcent retirer" << i <<"tentatives de colapse" << baseNbVertice-_mesh->n_edges() <<"vertices retirées";


    }
    else if(method == "Par angle")
    {

    }
    else if(method == "Par planéité")
    {

    }
    else
    {
        qDebug() << "Méthode inconnue !!!";
    }

}

/* **** début de la partie boutons et IHM **** */
void MainWindow::updateEdgeSelectionIHM()
{
    /* **** à compléter ! (Partie 3) ****
     * Cette fonction met à jour l'interface, les critères pourrons être affichés dans la zone de texte pour les vérifier
     */

    QString infos = "";
    infos = infos + "Surface : " + QString::number(0) + "\n";
    infos = infos + "C1 : " + QString::number(0) + "\n";
    infos = infos + "C2 : " + QString::number(0) + "\n";
    infos = infos + "C3 : " + QString::number(0) + "\n";
    ui->infoEdgeSelection->setPlainText(infos);

    ui->labelEdge->setText(QString::number(edgeSelection));

    // on montre la nouvelle sélection
    showEdgeSelection(&mesh);
}
/* **** fin de la partie à compléter **** */

void MainWindow::on_pushButton_edgeMoins_clicked()
{
    // mise à jour de l'interface
    edgeSelection = edgeSelection - 1;
    updateEdgeSelectionIHM();
}

void MainWindow::on_pushButton_edgePlus_clicked()
{
    // mise à jour de l'interface
    edgeSelection = edgeSelection + 1;
    updateEdgeSelectionIHM();
}

void MainWindow::on_pushButton_delSelEdge_clicked()
{
    // on supprime l'arête d'indice edgeSelection
    collapseEdge(&mesh, edgeSelection);

    // on actualise la sélection
    showEdgeSelection(&mesh);
}

void MainWindow::on_pushButton_chargement_clicked()
{
    // fenêtre de sélection des fichiers
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh"), "", tr("Mesh Files (*.obj)"));

    // chargement du fichier .obj dans la variable globale "mesh"
    OpenMesh::IO::read_mesh(mesh, fileName.toUtf8().constData());

    mesh.update_normals();

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);

    // on affiche le maillage
    displayMesh(&mesh);
}

void MainWindow::on_pushButton_decimate_clicked()
{
    decimation(&mesh, ui->horizontalSlider->value(), ui->comboBox->currentText());
    displayMesh(&mesh);
}
/* **** fin de la partie boutons et IHM **** */



/* **** fonctions supplémentaires **** */
// permet d'initialiser les couleurs et les épaisseurs des élements du maillage
void MainWindow::resetAllColorsAndThickness(MyMesh* _mesh)
{
    for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
    {
        _mesh->data(*curVert).thickness = 1;
        _mesh->set_color(*curVert, MyMesh::Color(0, 0, 0));
    }

    for (MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++)
    {
        _mesh->set_color(*curFace, MyMesh::Color(150, 150, 150));
    }

    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
    {
        _mesh->data(*curEdge).thickness = 1;
        _mesh->set_color(*curEdge, MyMesh::Color(0, 0, 0));
    }
}

// charge un objet MyMesh dans l'environnement OpenGL
void MainWindow::displayMesh(MyMesh* _mesh, bool isTemperatureMap, float mapRange)
{
    GLuint* triIndiceArray = new GLuint[_mesh->n_faces() * 3];
    GLfloat* triCols = new GLfloat[_mesh->n_faces() * 3 * 3];
    GLfloat* triVerts = new GLfloat[_mesh->n_faces() * 3 * 3];

    int i = 0;

    if(isTemperatureMap)
    {
        QVector<float> values;

        if(mapRange == -1)
        {
            for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
                values.append(fabs(_mesh->data(*curVert).value));
            qSort(values);
            mapRange = values.at(values.size()*0.8);
            qDebug() << "mapRange" << mapRange;
        }

        float range = mapRange;
        MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
        MyMesh::ConstFaceVertexIter fvIt;

        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = _mesh->cfv_iter(*fIt);
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            if(_mesh->data(*fvIt).value > 0){triCols[3*i+0] = 255; triCols[3*i+1] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+2] = 255 - std::min((_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            else{triCols[3*i+2] = 255; triCols[3*i+1] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0); triCols[3*i+0] = 255 - std::min((-_mesh->data(*fvIt).value/range) * 255.0, 255.0);}
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++;
        }
    }
    else
    {
        MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
        MyMesh::ConstFaceVertexIter fvIt;
        for (; fIt!=fEnd; ++fIt)
        {
            fvIt = _mesh->cfv_iter(*fIt);
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++; ++fvIt;
            triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
            triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
            triIndiceArray[i] = i;

            i++;
        }
    }


    ui->displayWidget->loadMesh(triVerts, triCols, _mesh->n_faces() * 3 * 3, triIndiceArray, _mesh->n_faces() * 3);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

    GLuint* linesIndiceArray = new GLuint[_mesh->n_edges() * 2];
    GLfloat* linesCols = new GLfloat[_mesh->n_edges() * 2 * 3];
    GLfloat* linesVerts = new GLfloat[_mesh->n_edges() * 2 * 3];

    i = 0;
    QHash<float, QList<int> > edgesIDbyThickness;
    for (MyMesh::EdgeIter eit = _mesh->edges_begin(); eit != _mesh->edges_end(); ++eit)
    {
        float t = _mesh->data(*eit).thickness;
        if(t > 0)
        {
            if(!edgesIDbyThickness.contains(t))
                edgesIDbyThickness[t] = QList<int>();
            edgesIDbyThickness[t].append((*eit).idx());
        }
    }
    QHashIterator<float, QList<int> > it(edgesIDbyThickness);
    QList<QPair<float, int> > edgeSizes;
    while (it.hasNext())
    {
        it.next();

        for(int e = 0; e < it.value().size(); e++)
        {
            int eidx = it.value().at(e);

            MyMesh::VertexHandle vh1 = _mesh->to_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh1)[0];
            linesVerts[3*i+1] = _mesh->point(vh1)[1];
            linesVerts[3*i+2] = _mesh->point(vh1)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;

            MyMesh::VertexHandle vh2 = _mesh->from_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh2)[0];
            linesVerts[3*i+1] = _mesh->point(vh2)[1];
            linesVerts[3*i+2] = _mesh->point(vh2)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;
        }
        edgeSizes.append(qMakePair(it.key(), it.value().size()));
    }

    ui->displayWidget->loadLines(linesVerts, linesCols, i * 3, linesIndiceArray, i, edgeSizes);

    delete[] linesIndiceArray;
    delete[] linesCols;
    delete[] linesVerts;

    GLuint* pointsIndiceArray = new GLuint[_mesh->n_vertices()];
    GLfloat* pointsCols = new GLfloat[_mesh->n_vertices() * 3];
    GLfloat* pointsVerts = new GLfloat[_mesh->n_vertices() * 3];

    i = 0;
    QHash<float, QList<int> > vertsIDbyThickness;
    for (MyMesh::VertexIter vit = _mesh->vertices_begin(); vit != _mesh->vertices_end(); ++vit)
    {
        float t = _mesh->data(*vit).thickness;
        if(t > 0)
        {
            if(!vertsIDbyThickness.contains(t))
                vertsIDbyThickness[t] = QList<int>();
            vertsIDbyThickness[t].append((*vit).idx());
        }
    }
    QHashIterator<float, QList<int> > vitt(vertsIDbyThickness);
    QList<QPair<float, int> > vertsSizes;

    while (vitt.hasNext())
    {
        vitt.next();

        for(int v = 0; v < vitt.value().size(); v++)
        {
            int vidx = vitt.value().at(v);

            pointsVerts[3*i+0] = _mesh->point(_mesh->vertex_handle(vidx))[0];
            pointsVerts[3*i+1] = _mesh->point(_mesh->vertex_handle(vidx))[1];
            pointsVerts[3*i+2] = _mesh->point(_mesh->vertex_handle(vidx))[2];
            pointsCols[3*i+0] = _mesh->color(_mesh->vertex_handle(vidx))[0];
            pointsCols[3*i+1] = _mesh->color(_mesh->vertex_handle(vidx))[1];
            pointsCols[3*i+2] = _mesh->color(_mesh->vertex_handle(vidx))[2];
            pointsIndiceArray[i] = i;
            i++;
        }
        vertsSizes.append(qMakePair(vitt.key(), vitt.value().size()));
    }

    ui->displayWidget->loadPoints(pointsVerts, pointsCols, i * 3, pointsIndiceArray, i, vertsSizes);

    delete[] pointsIndiceArray;
    delete[] pointsCols;
    delete[] pointsVerts;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    vertexSelection = -1;
    edgeSelection = -1;
    faceSelection = -1;

    modevoisinage = false;

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

