;;;; Utility functions for CMSC 471 HW 2
;;;;
;;;; Author:  Eric Eaton
;;;;


;;;; City Data Structure
(defstruct city
	(name (make-symbol))
	(neighbors nil)
	(dist)
	(visited nil))



;;;; Global Hash Table for City Data
(defparameter *all-cities* (make-hash-table))



;;;; File Reader Functions

;;; Function:  read-cities
;;; Arguments:
;;;  filename - the name of the cities file
;;; Reads in the city data and the straight line distance to the goal city
(defun read-cities (filename &aux stream line (name nil) (dist nil) pos1)
  "Reads in the city data and the straight line distance to the goal city."

  ;; loop through the file, parsing each line
  (with-open-file (stream filename :direction :input)
		  (do ((line (read-line stream nil 'eof)
			     (read-line stream nil 'eof)))
		      ((eql line 'eof))
		      ;; create a city object with the name and distance specified
		      ;; in the line and hash the city into the hash table
		      (progn (multiple-value-setq (name pos1) (read-from-string line))
			     (setf dist (read-from-string line nil nil :start pos1))
			     ;; error check for no name and dist pair
			     (if (or (null name) (null dist))
				 (progn (format t "Invalid file format.~%")
					(return nil)))
			     (setf (gethash name *all-cities*) (make-city :name name
						    :dist dist))))))
			      

;;; Function:  read-neighbors
;;; Arguments:
;;;   filename - the name of the neighbors file
;;; Reads in the neighbors for each city
(defun read-neighbors (filename &aux stream line city neighbor dist pos1)
  "Reads in the neighbors for each city."

  ;; loop through the file, parsing each line
  (with-open-file (stream filename :direction :input)
		  (do ((line (read-line stream nil 'eof)
			     (read-line stream nil 'eof)))
		      ((eql line 'eof))

		      ;; add the neighboring city and the distance to the city
		      ;; object specified by name in the line
		      (progn (multiple-value-setq (city_name pos1) (read-from-string line))
			     (multiple-value-setq (neighbor pos1)
			       (read-from-string line nil nil :start pos1))
			     (setf dist (read-from-string line nil nil :start pos1))

			     ;; check for any unspecified values of the triple
			     (if (or (null city_name) (null neighbor) (null dist))
				 (progn (format t "Invalid file format.~%")
					(return nil)))

			     ;; check for path from a city to itself
			     ;; if the path exists, print a warning
			     ;; else, add the neighbor to the appropriate city's list
			     (if (equal city_name neighbor)
				 (progn
				   (format t "Warning:  File has a path from ~A to" city_name)
				   (format t " itself.  This path has been ignored.~%"))
			         (progn
				   (if (null (get-city-struct city_name))
				       (progn (format t "Warning:  No entry for ~A" city_name)
					      (format t " in the hash table already.~%")
					      (return nil))
				       (setf (city-neighbors (get-city-struct city_name)) 
					     (append (city-neighbors 
						      (get-city-struct city_name)) 
						     (list (cons neighbor dist)))))
				   (if (null (get-city-struct neighbor))
				       (progn (format t "Warning:  No entry for ~A" neighbor)
					      (format t " in the hash table already.~%")
					      (return nil))
				       (setf (city-neighbors (get-city-struct neighbor))
					     (append (city-neighbors 
						      (get-city-struct neighbor))
						     (list (cons city_name dist)))))))))))



;;;; Map Manipulation Functions


;;; Function:  all-cities
;;; Returns a list of all of the cities on the map
(defun all-cities (&aux (cities nil))
  "Returns a list of all the cities on the map."

  ;; apply function to accumulate all city names in a list
  (maphash #'(lambda (k v)
	       (setf cities (append cities (list (city-name v))))) *all-cities*)

  ;; return the list of cities
  (return-from all-cities cities))



;;; Function:  get-city-struct
;;; Arguments:
;;;   city_name - the name of the city
;;; Returns the city structure for the specified city
(defun get-city-struct (city_name)
  "Returns the city structure for the specified city."

  ;; error check to make certain that the city exists
  (if (null (gethash city_name *all-cities*))
      (progn (format t "~A not in table of cities.~%" city_name)
	     (return-from get-city-struct nil)))

  ;; return the structure for the city
  (gethash city_name *all-cities*))



;;; Function:  city-straight-line-distance
;;; Arguments:
;;;   city_name - the name of the city
;;; Returns the straight-line distance from the city to the goal.
(defun city-straight-line-distance (city_name)
  "Returns the straight-line distance from the city to the goal city."
  (city-dist (get-city-struct city_name)))



;;; Function:  city-neighbor-list
;;; Arguments:
;;;   city_name - the name of the city
;;; Returns a list of neighbors for the specified city
(defun city-neighbor-list (city_name &aux (cities nil))
  "Returns a list of neighbors for the specified city."

  ;; error check to make certain that the city exists
  (if (null (gethash city_name *all-cities*))
      (progn (format t "~A not in table of cities.~%" city_name)
	     (return-from city-neighbor-list nil)))

  ;; apply function to neighbor list to gather names of cities
  (mapcar #'(lambda(x) 
	       (setf cities (append (list (car x)) cities)))
	  (city-neighbors (gethash city_name *all-cities*)))

  ;; return list of city names
  (return-from city-neighbor-list cities))



;;; Function:  neighbors-p
;;; Arguments:
;;;   city_name1 - the name of one city
;;;   city_name2 - the name of another city
;;; Returns the distance between the two specified cities
;;; if they are neighbors, or nil, if they are not
(defun neighbors-p (city_name1 city_name2)
  "Returns the distance between the two specified cities if they are neighbors; nil if they are not neighbors."

  ;; error check to make certain that the cities exists
  (if (null (gethash city_name1 *all-cities*))
      (progn (format t "~A not in table of cities.~%" city_name1)
	     (return-from neighbors-p nil)))
  (if (null (gethash city_name2 *all-cities*))
      (progn (format t "~A not in table of cities.~%" city_name2)
	     (return-from neighbors-p nil)))

  ;; find the distance associated with the two cities
  (cdr (assoc city_name2 (city-neighbors (get-city-struct city_name1)))))



;;; Function:  closest-neighbor
;;; Arguments: 
;;;   city_name - the name of the city
;;; Returns the name of the closest direct neighbor to the specified city
(defun closest-neighbor (city_name &aux (city nil) (dist nil))
  "Returns the name of the closest direct neighbor to the specified city."

  ;; error check to make certain that the city exists
  (if (null (gethash city_name *all-cities*))
      (progn (format t "~A not in table of cities.~%" city_name)
	     (return-from closest-neighbor nil)))

  ;; apply a function to keep tracking the city with the least
  ;; distance from the specified city
  (mapcar #'(lambda (x)
	       (if (or (null dist) (< (cdr x) dist))
		   (progn (setf city (car x))
			  (setf dist (cdr x)))))
	   (city-neighbors (get-city-struct city_name)))

  ;; return the name of the city with the closest neighbor
  (return-from closest-neighbor city))



;;; Function:  neighbors-within-distance
;;; Arguments:
;;;   city_name - the name of the city
;;;   distance  - the distance from the city
;;; Returns a list of all direct neighbors of the specified 
;;;   city that are within the specified distance
(defun neighbors-within-distance (city_name distance &aux (cities nil))
  "Returns a list of all direct neighbors of the specified city within the specified distance."

  ;; error check that distance is a positive real number
  (if (or (not (integerp distance)) (< distance 0))
    (progn (format t "distance is not a positive real number")
	   (return-from neighbors-within-distance nil)))

  ;; error check to make certain that the city exists
  (if (null (gethash city_name *all-cities*))
      (progn (format t "~A not in table of cities.~%" city_name)
	     (return-from neighbors-within-distance nil)))

  ;; accumulate all cities within the specified distance
  (mapcar #'(lambda (x)
	      (if (<= (cdr x) distance)
		(setf cities (append cities (list (car x))))))
	  (city-neighbors (get-city-struct city_name)))

  ;; return the list of cities within the distance
  (return-from neighbors-within-distance cities))



;;; Function:  hw2
;;; Script of function calls to output the results table for Homework 2
(defun hw2 ()
  (format t "Complete this function to load and intialize all data structures, run all of the searches, and output the results table!!~%"))


(defstruct city-node
  	(name)
	(parent nil)
	(children nil))


(defun BF-search (city_name)
  (mapcar #'(lambda (x)
	      (setf (city-visited x) nil))
	  (all-cities))

  (let ((nodes (list (make-city-node :name city_name))))
    (loop
      (if (null nodes)
	(return-from BF-search nil))
      (let ((node (pop nodes)))
	(if (eq (city-node-name node) 'bucharest)
	  (return-from BF-search node) 	;; TODO: reconstruct path
	  (setf (city-visited (get-city-struct (city-node-name node))) T)
	(setf nodes (append nodes (city-neighbor-list 'pitesti))))))) ;; TODO: city-neighbor-list, remove visited, make nodes out of the rest, append to queue.
