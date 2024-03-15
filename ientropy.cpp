#include "ientropy.h"
// #include <cv.h>
#include <algorithm>
#include <iostream>


template <typename T> vector<size_t> sortIndexes(const vector<T> &v) {
  vector<size_t> idx(v.size());
  for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

  sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

double vectorAngle(Point c, Point p1, Point p2) {
    double dotp;
    double angle;
    
    dotp = (p1.x - c.x) * (p2.x - c.x) + (p1.y - c.y) * (p2.y - c.y);
    angle = dotp/(sqrt(pow(p1.x - c.x,2) + pow(p1.y - c.y, 2)) * sqrt(pow(p2.x - c.x, 2) + pow(p2.y - c.y, 2)));
    
    if (angle >= 1) {
        return 0;
    } else if (angle <= -1) {
        return M_PI;
    }
    
    return acos(angle);
}

//Calculate eye entropy; de: distance entropy; ae: angle entropy
double calEntropy(Point cpt_img, std::vector<Point> cpt, double &de, double &ae, int inc, int rankType) {
    std::vector<double> vrow(cpt.size());
    std::vector<size_t> vid(cpt.size());
    std::vector<size_t> subvid(inc + 50);
    std::vector<size_t> newvid;
    std::vector<double> vde(cpt.size());
    std::vector<double> subvde(inc + 50);
    std::vector<double> vae(cpt.size());
    std::vector<double> subvae(inc + 50);
    std::vector<double> ve(cpt.size());
    std::vector<double> subve(inc + 50);
    std::vector<double> dc(cpt.size()); //vector to record the distance from each ommatidia to the center of the eye
    std::vector<double> angle(5);
    std::vector<size_t> aid(cpt.size());
    double e;
    int i, j, k, n; 
    
    for (i = 0; i < cpt.size(); i ++) {
        for (j = 0; j < cpt.size(); j ++) {
            vrow[j] = sqrt(pow(cpt[i].x - cpt[j].x, 2) + pow(cpt[i].y - cpt[j].y, 2));
        }
        dc[i] = sqrt(pow(cpt[i].x - cpt_img.x, 2) + pow(cpt[i].y - cpt_img.y, 2));
        vid = sortIndexes(vrow);
        
        vde[i] = 0;
        for (j = 2; j < 7; j ++) {
            vde[i] += (vrow[vid[j]]/vrow[vid[1]] - 1);
        }
        
        for (j = 1; j < 7; j ++) {
            n = 0;
            for (k = 1; k < 7; k ++) {
                if (j != k) {
                    angle[n++] = vectorAngle(cpt[vid[0]], cpt[vid[j]], cpt[vid[k]]);
                }
            }
            aid = sortIndexes(angle);
            vae[i] += abs(M_PI/3 - angle[aid[0]]) + abs(M_PI/3 - angle[aid[1]]);
        }
        
        ve[i] = log(vde[i] + vae[i]); 
    } 
    
    if (rankType == 1) {
        vid = sortIndexes(ve);
        for (i = 0; i < (inc < ve.size() ? inc : ve.size()); i ++) {
            e += ve[vid[i]];
            de += vde[vid[i]];
            ae += vae[vid[i]];
        }
    } else {
        vid = sortIndexes(dc);
        if (rankType == 2) {
            for (i = 0; i < (inc < ve.size() ? inc : ve.size()); i ++) {
                e += ve[vid[i]];
                de += vde[vid[i]];
                ae += vae[vid[i]];
            }
        } else {
            if ((inc + 50) < ve.size()) {
                subvid.assign(vid.begin(), vid.begin() + inc + 50);
                for (i = 0; i < subvid.size(); i++) {
                    subve[i] = ve[subvid[i]];
                    subvae[i] = vae[subvid[i]];
                    subvde[i] = vde[subvid[i]];
                }
            } else {
                subve.assign(ve.begin(), ve.end());
                subvae.assign(vae.begin(), vae.end());
                subvde.assign(vde.begin(), vde.end());
            }
            newvid = sortIndexes(subve);
    
            for (i = 0; i < (inc < ve.size() ? inc : ve.size()); i ++) {
                e += subve[newvid[i]];
                de += subvde[newvid[i]];
                ae += subvae[newvid[i]];
            }
        }
    }
    
    if (inc > ve.size()) {
        e = inc * e/ve.size();
        de = inc * de/ve.size();
        ae = inc * ae/ve.size();
     }
    
    return e;
}

//calculate phynotypic score based on eye entropy and infusion index
double phynoScore(Point cpt_img, std::vector<Point> cpt, int z, int inc, double &de, double &ae, double &e, int rankType) {
    e = calEntropy(cpt_img, cpt, de, ae, inc, rankType);
    
    return exp(-log(log(z)))*e;
}
