;;; LeeJamesHW1.lisp
;;; James Lee <jlee23@umbc.ed>
;;; 20070917

(defun nextint (n)
  ;; for integers
  (if (integerp n)
    ;; return the value of it plus 1
    (+ n 1)))

(defun fact (n)
  ;; for natural numbers 
  (if (and (integerp n)	(>= n 0))
    ;; multiply itself with the factorial of itself minus 1
    ;; until 0, which just equals 1
    (cond ((= n 0) 1)
	  (t (* n (fact (- n 1)))))))

(defun my-third (l)
  ;; for a list with three or more elements
  (if (and (listp l) (>= (list-length l) 3))
    ;; pop the list twice and return the car
    (dotimes (i 2 (car l)) (pop l))))

(defun scale-by-two (l)
  ;; for lists
  (if (listp l)
    ;; return a list where each integer element has been doubled
    (mapcar #'(lambda (x) (if (integerp x) (* x 2))) l)))

(defun posint-mapcar (l)
  ;; for lists
  (if (listp l)
    ;; initialize an empty list of positive integers
    (let ((posints nil))
      ;; for each element in the list
      (mapcar #'(lambda (x)
		  	;; if the element is another list
		  (cond ((listp x)
			 ;; call posint-mapcar on the list
			 ;; and append it to the list of positive integers
			 (setf posints (append posints (posint-mapcar x))))
			;; if the element is a positive integer
			((and (integerp x) (> x 0))
			 ;; just append it to the list of positive integers
			 (setf posints (append posints (cons x '())))))) l)
      ;; and return the list of positive integers
      posints)))

(defun posint-loop (l)
  ;; for lists
  (if (listp l)
    ;; initialize an empty list of positive integers
    (let ((posints nil))
      ;; loop through each element in the list
      (loop for i in l do
		  ;; if the element is another list
	    (cond ((listp i)
		   ;; call posint-mapcar on the list
		   ;; and append it to the list of positive integers
		   (setf posints (append posints (posint-loop i))))
		  ;; if the element is a positive integer
		  ((and (integerp i) (> i 0))
		   ;; just append it to the list of positive integers
		   (setf posints (append posints (cons i '()))))))
      ;; and return the list of positive integers
      posints)))

(defun posint-recursive (l)
  ;; while l is not nil
  (if l
    (let ((a (car l)))
      (cond
	;; if the car of the list is a list
	((listp a)
	 ;; return a list of positive integers in this list and the rest of the
	 ;; elements
	 (append (posint-recursive a) (posint-recursive (cdr l))))
	;; if the element is a positive integer
	((and (integerp a) (> a 0))
	 ;; return a list of it and add the rest of the positive integers
	 (append (cons a '()) (posint-recursive (cdr l))))
	;; if its neither a list nor a positive integer, we still want to 
	;; process the rest of the list
	(t
	  (posint-recursive (cdr l)))))))

(defun case? (s)
  ;; for strings
  (if (stringp s)
    	  ;; if the string is equal to its uppercase version, then it is
	  ;; uppercase
    (cond ((string= s (string-upcase s)) 'upper)
	  ;; if the string is equal to its lowercase version, then it is
	  ;; lowercase
	  ((string= s (string-downcase s)) 'lower)
	  ;; otherwise it is mixed
	  (t 'mixed))))

(defun flatten-tree (l)
  ;; while l is not nil
  (if l
    ;; if the first element is an atom
    (if (atom (car l))
      ;; return a list of it and the rest of the tree flattened
      (cons (car l) (flatten-tree (cdr l)))
      ;; otherwise, flatten the first element, then the rest of the tree and
      ;; return it.  append will put the values of two lists into a new list
      ;; where cons will just put the two lists into a new list.
      (append (flatten-tree (car l)) (flatten-tree (cdr l))))))
